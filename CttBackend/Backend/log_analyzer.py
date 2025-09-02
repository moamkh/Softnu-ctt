import os
import json
from datetime import datetime, timezone, timedelta
import re
from typing import Dict, List, Optional, Set
import logging
from database import DatabaseController
from models import User, CallSummary
from colorama import init, Fore, Style

# Initialize colorama
init()

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
logger = logging.getLogger(__name__)

IRAN_TZ = timezone(timedelta(hours=3, minutes=30))

class LogAnalyzer:
    def __init__(self, logs_dir: str = "C:/Users/amin/Desktop/NovinAmiVoip/AmiController/logs"):
        self.logs_dir = logs_dir
        self.db = DatabaseController()
        self.processed_extensions = set()
        logger.info(f"Initialized LogAnalyzer with logs directory: {self.logs_dir}")

    def _extract_extension_number(self, filename: str) -> Optional[str]:
        """Extract extension number from log filename"""
        match = re.search(r'call_logs_(\d+)\.log', filename)
        extension = match.group(1) if match else None
        if extension:
            logger.debug(f"Extracted extension number {extension} from filename {filename}")
        else:
            logger.warning(f"Could not extract extension number from filename {filename}")
        return extension

    def _extract_extension_from_call_flow(self, line: str) -> Optional[str]:
        """Extract extension number from call flow marker line"""
        # Match patterns like:
        # "----------flow of a call made from 09136421196 to 413---------- incoming"
        # "----------flow of a call made from 413 to 09136421196---------- outgoing"
        # "---------- flow of a call made from 513 to 413 ---------- incoming that was not awnsered"
        match = re.search(r'from\s+(\d+)\s+to\s+(\d+)', line)
        if match:
            # Return both extensions found in the line
            return [match.group(1), match.group(2)]
        return None

    def _create_dummy_user(self, extension_number: str) -> User:
        """Create a dummy user for the extension if it doesn't exist"""
        try:
            logger.info(f"Checking for existing user with extension {extension_number}")
            existing_user = self.db.get_user_by_extension(int(extension_number))
            if existing_user:
                logger.info(f"Found existing user for extension {extension_number}")
                return existing_user

            logger.info(f"Creating new user for extension {extension_number}")
            fullname = f"User {extension_number}"
            user = self.db.create_user(
                fullname=fullname,
                extension_number=int(extension_number),
                role="CTT user"
            )
            if user:
                logger.info(f"Successfully created new user: {fullname}")
                return user
            else:
                print(f"{Fore.RED}Failed to create user for extension {extension_number}{Style.RESET_ALL}")
                return None
        except Exception as e:
            print(f"{Fore.RED}Error creating user for extension {extension_number}: {str(e)}{Style.RESET_ALL}")
            logger.error(f"Error creating user for extension {extension_number}: {str(e)}")
            return None

    def _parse_call_data(self, call_events: List[Dict]) -> Optional[Dict]:
        """Parse call events and extract relevant data"""
        try:
            logger.debug("Starting to parse call data")
            
            # Find required events
            start_event = next((e for e in call_events if e.get('type') == 'call_started'), None)
            end_event = next((e for e in call_events if e.get('type') == 'call_ended'), None)
            summary_event = next((e for e in call_events if e.get('type') == 'call_summary'), None)
            
            # Check for ringing event to determine if it's an incoming call
            has_ringing = any(e.get('type') == 'call_ringing' for e in call_events)
            
            if not all([start_event, end_event, summary_event]):
                logger.warning("Missing required events in call data")
                return None

            # Convert timestamps to epoch
            start_time = int(datetime.fromisoformat(start_event['timestamp']).replace(tzinfo=IRAN_TZ).timestamp())
            end_time = int(datetime.fromisoformat(end_event['timestamp']).replace(tzinfo=IRAN_TZ).timestamp())
            duration = int(float(summary_event['call_duration']))

            # Get the extension number from caller_id in call_started event
            extension_number = int(start_event['caller_id'])
            
            # Get the other party's number from call_summary
            callee = summary_event['callee']
            
            # Determine if it's an internal call (both numbers are short)
            is_internal = len(str(extension_number)) <= 4 and len(callee) <= 4
            call_type = 'internal' if is_internal else 'external'
            
            # Determine direction based on presence of ringing event
            direction = 'in' if has_ringing else 'out'

            call_data = {
                'start_time': start_time,
                'end_time': end_time,
                'duration': duration,
                'callee': callee,
                'direction': direction,
                'type': call_type,
                'extension_number': extension_number
            }
            
            logger.info(f"Successfully parsed call data: {call_type} {direction} call from {extension_number} to {callee}, duration: {duration}s")
            logger.debug(f"Call data details: {call_data}")
            return call_data
        except Exception as e:
            print(f"{Fore.RED}Error parsing call data: {str(e)}{Style.RESET_ALL}")
            logger.error(f"Error parsing call data: {str(e)}")
            logger.error(f"Call events: {call_events}")
            return None

    def _extract_extensions_from_file(self, filepath: str) -> Set[str]:
        """Extract all unique extension numbers from a log file"""
        extensions = set()
        try:
            with open(filepath, 'r') as f:
                for line in f:
                    line = line.strip()
                    if line.startswith('----------'):
                        # Extract extension from call flow marker
                        ext_numbers = self._extract_extension_from_call_flow(line)
                        if ext_numbers:
                            extensions.update(ext_numbers)
        except Exception as e:
            logger.error(f"Error extracting extensions from file {filepath}: {str(e)}")
        return extensions

    def process_log_file(self, filepath: str) -> None:
        """Process a single log file"""
        try:
            logger.info(f"Starting to process log file: {filepath}")
            
            # Extract extension from filename
            extension_number = self._extract_extension_number(os.path.basename(filepath))
            if not extension_number:
                print(f"{Fore.RED}Could not process file {filepath}: Invalid filename format{Style.RESET_ALL}")
                logger.error(f"Could not process file {filepath}: Invalid filename format")
                return

            # Create user if not exists
            user = self._create_dummy_user(extension_number)
            if not user:
                print(f"{Fore.RED}Could not process file {filepath}: Failed to create/get user{Style.RESET_ALL}")
                logger.error(f"Could not process file {filepath}: Failed to create/get user")
                return

            # Now process the call summaries
            current_call_events = []
            call_count = 0
            json_buffer = ""
            in_json = False

            with open(filepath, 'r') as f:
                logger.info(f"Reading file {filepath} for call summaries")
                for line_num, line in enumerate(f, 1):
                    line = line.strip()
                    if not line:
                        continue

                    # Skip comment lines
                    if line.startswith('----------'):
                        continue

                    # Check for JSON start
                    if line.startswith('{'):
                        in_json = True
                        json_buffer = line
                    # Continue JSON if we're in a JSON block
                    elif in_json:
                        json_buffer += line
                    # Check for JSON end
                    if line.endswith('}'):
                        try:
                            event = json.loads(json_buffer)
                            if event.get('type') != 'monitor_initialized':
                                current_call_events.append(event)
                                if event.get('type') == 'call_ended':
                                    call_data = self._parse_call_data(current_call_events)
                                    if call_data:
                                        try:
                                            result = self.db.create_call_summary(**call_data)
                                            if result:
                                                call_count += 1
                                                logger.info(f"Successfully inserted call summary for call from {call_data['extension_number']} to {call_data['callee']}")
                                            else:
                                                print(f"{Fore.RED}Failed to insert call summary: Database returned None{Style.RESET_ALL}")
                                        except Exception as e:
                                            print(f"{Fore.RED}Failed to insert call summary: {str(e)}{Style.RESET_ALL}")
                                            logger.error(f"Failed to insert call summary: {str(e)}")
                                    current_call_events = []
                        except json.JSONDecodeError:
                            print(f"{Fore.RED}Invalid JSON at line {line_num} in file {filepath}{Style.RESET_ALL}")
                            logger.warning(f"Invalid JSON at line {line_num} in file {filepath}")
                        json_buffer = ""
                        in_json = False

            logger.info(f"Finished processing file {filepath}: Processed {call_count} calls")
            if call_count == 0:
                print(f"{Fore.RED}No calls were processed from file {filepath}{Style.RESET_ALL}")

        except Exception as e:
            print(f"{Fore.RED}Error processing file {filepath}: {str(e)}{Style.RESET_ALL}")
            logger.error(f"Error processing file {filepath}: {str(e)}")

    def analyze_logs(self) -> None:
        """Analyze all log files in the directory"""
        try:
            logger.info(f"Starting log analysis in directory: {self.logs_dir}")
            if not os.path.exists(self.logs_dir):
                print(f"{Fore.RED}Logs directory does not exist: {self.logs_dir}{Style.RESET_ALL}")
                logger.error(f"Logs directory does not exist: {self.logs_dir}")
                return

            log_files = [f for f in os.listdir(self.logs_dir) if f.startswith('call_logs_') and f.endswith('.log')]
            logger.info(f"Found {len(log_files)} log files to process")
            
            total_calls = 0
            for filename in log_files:
                filepath = os.path.join(self.logs_dir, filename)
                logger.info(f"Processing {filename}...")
                self.process_log_file(filepath)
                
            logger.info(f"Log analysis completed successfully. Processed {len(self.processed_extensions)} unique extensions")
            if total_calls == 0:
                print(f"{Fore.RED}No calls were processed from any files{Style.RESET_ALL}")
        except Exception as e:
            print(f"{Fore.RED}Error analyzing logs: {str(e)}{Style.RESET_ALL}")
            logger.error(f"Error analyzing logs: {str(e)}")

if __name__ == "__main__":
    logger.info("Starting log analyzer script")
    analyzer = LogAnalyzer()
    analyzer.analyze_logs()
    logger.info("Log analyzer script completed") 