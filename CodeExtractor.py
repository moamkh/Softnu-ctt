import os
import fnmatch

# ==============================================================================
# ======================== SCRIPT CONFIGURATION ================================
# ==============================================================================

# 1. FOLDERS_TO_SCAN
#    List the root folders you want to scan for code.
#    You can use '.' to represent the current directory where the script is run.
#    Example: ['src', 'tests', 'scripts']
FOLDERS_TO_SCAN = [
    '.'
]

# 2. FILE_EXTENSIONS_TO_INCLUDE
#    List the file extensions you want to include in the output.
#    Use the format "*.extension" (e.g., "*.py", "*.js").
#    This uses "glob" patterns, so you can also use patterns like "Dockerfile".
FILE_PATTERNS_TO_INCLUDE = [
    "*.cpp",
    "*.h",
    "*.hpp",
    "*.pro",
]

# 3. ITEMS_TO_IGNORE
#    List of directory names, file names, or patterns to ignore.
#    This is also a "glob" pattern.
#    - To ignore a directory and everything inside: 'node_modules'
#    - To ignore a specific file: 'package-lock.json'
#    - To ignore files by extension: '*.log'
#    - To ignore a specific path: 'dist/*'
ITEMS_TO_IGNORE = [
    '.git',
    '.vscode',
    'etc',
    'deployment',
    'CTT.pro.user',
    
]

# 4. OUTPUT_FILENAME
#    The name of the single text file where all the code will be consolidated.
OUTPUT_FILENAME = "project_context.txt"

# ==============================================================================
# ======================== END OF CONFIGURATION ================================
# ==============================================================================


def create_code_context():
    """
    Recursively finds files, filters them, and consolidates their content
    into a single output file based on the script's configuration.
    """
    processed_files_count = 0
    print("Starting the code consolidation process...")

    # Ensure the script itself is not included in the output
    script_name = os.path.basename(__file__)
    if script_name not in ITEMS_TO_IGNORE:
        ITEMS_TO_IGNORE.append(script_name)
    if OUTPUT_FILENAME not in ITEMS_TO_IGNORE:
        ITEMS_TO_IGNORE.append(OUTPUT_FILENAME)

    try:
        with open(OUTPUT_FILENAME, 'w', encoding='utf-8') as outfile:
            for folder in FOLDERS_TO_SCAN:
                folder_abs_path = os.path.abspath(folder)
                if not os.path.isdir(folder_abs_path):
                    print(f"Warning: Folder '{folder_abs_path}' does not exist. Skipping.")
                    continue

                print(f"Scanning folder: {folder_abs_path}...")

                for dirpath, dirnames, filenames in os.walk(folder_abs_path, topdown=True):
                    # --- Filter directories to ignore ---
                    # The 'topdown=True' in os.walk allows us to modify dirnames in-place
                    # to prevent the walk from descending into ignored directories.
                    dirnames[:] = [d for d in dirnames if d not in ITEMS_TO_IGNORE]

                    # --- Filter and process files ---
                    for filename in filenames:
                        # Check if the file itself should be ignored
                        if any(fnmatch.fnmatch(filename, pattern) for pattern in ITEMS_TO_IGNORE):
                            continue

                        # Check if the file extension is in our include list
                        if not any(fnmatch.fnmatch(filename, pattern) for pattern in FILE_PATTERNS_TO_INCLUDE):
                            continue

                        full_path = os.path.join(dirpath, filename)

                        try:
                            # Use relative path for cleaner output if starting from '.'
                            if folder == '.':
                                display_path = os.path.relpath(full_path)
                            else:
                                display_path = full_path

                            outfile.write(f"--- FILE: {display_path} ---\n\n")

                            with open(full_path, 'r', encoding='utf-8', errors='ignore') as infile:
                                outfile.write(infile.read())

                            outfile.write("\n\n" + "="*80 + "\n\n")
                            processed_files_count += 1
                            print(f"  + Added: {display_path}")

                        except Exception as e:
                            print(f"  - Error reading file {full_path}: {e}")

    except IOError as e:
        print(f"\nError: Could not write to output file {OUTPUT_FILENAME}. Reason: {e}")
        return

    print("\n--------------------------------------------------")
    print(f"Success! Processed {processed_files_count} files.")
    print(f"Consolidated codebase is available at: {os.path.abspath(OUTPUT_FILENAME)}")
    print("--------------------------------------------------")


if __name__ == "__main__":
    create_code_context()
