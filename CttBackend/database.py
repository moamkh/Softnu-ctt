from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, Session, joinedload
from sqlalchemy.exc import SQLAlchemyError
from typing import List, Optional, Dict, Any
from datetime import datetime
import os
from models import Base, User, CallSummary, RelUserCalls
from sqlalchemy import or_




class DatabaseController:
    def __init__(self, db_url: str = None):
        if db_url is None:
            # Use PostgreSQL connection
            db_url = (
                "postgresql://postgres:softnou@192.168.20.67:5432/ctt_database"
            )
            print(f"Using PostgreSQL database at: {db_url}")
        self.engine = create_engine(db_url)
        self.SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=self.engine)
        
        # Only create tables if they don't exist
        if not os.path.exists(db_url):
            print(f"Creating new database at: {db_url}")
            Base.metadata.create_all(bind=self.engine)
        else:
            print(f"Using existing database at: {db_url}")

    def get_db(self) -> Session:
        db = self.SessionLocal()
        try:
            return db
        finally:
            db.close()

    def create_user(self, fullname: Optional[str], extension_number: str, username: Optional[str] = None, role: str = "CTT user") -> Optional[User]:
        db = self.get_db()
        try:
            # Check if user with extension already exists
            existing_user = db.query(User).filter(User.extension_number == extension_number).first()
            if existing_user:
                print(f"User with extension {extension_number} already exists")
                return existing_user

            user = User(fullname=fullname, extension_number=extension_number, username=username, role=role)
            db.add(user)
            db.commit()
            db.refresh(user)
            print(f"Created new user: {user.fullname} with extension {user.extension_number}")
            return user
        except SQLAlchemyError as e:
            db.rollback()
            print(f"Error creating user: {str(e)}")
            raise e
        finally:
            db.close()

    def get_user_by_extension(self, extension_number: int) -> Optional[User]:
        db = self.get_db()
        try:
            return db.query(User).filter(User.extension_number == str(extension_number)).first()
        finally:
            db.close()

    def get_call_summary_by_id(self, call_id: int) -> Optional[CallSummary]:
        db = self.get_db()
        try:
            return db.query(CallSummary).options(joinedload(CallSummary.users)).filter(CallSummary.id == call_id).first()
        finally:
            db.close()

    def create_call_summary(self, 
                          start_time: int,
                          end_time: int,
                          duration: int,
                          callee: str,
                          direction: str,
                          type: str,
                          extension_number: str,
                          status: Optional[str] = None,
                          ai_response: Optional[str] = None,
                          unique_id: str = None) -> Optional[CallSummary]:
        """Create a new call summary and its relationship with the user"""
        db = self.get_db()
        try:
            # Create call summary
            call_summary = CallSummary(
                start_time=start_time,
                end_time=end_time,
                duration=duration,
                callee=callee,
                direction=direction,
                type=type,
                extension_number=extension_number,
                status=status,
                ai_response=ai_response,
                unique_id=unique_id
            )
            db.add(call_summary)
            db.flush()  # Flush to get the call_summary.id

            # Find user by extension number
            user = db.query(User).filter(User.extension_number == str(extension_number)).first()
            
            if user:
                # Create relationship
                rel = RelUserCalls(user_id=user.id, call_summary_id=call_summary.id)
                db.add(rel)
                db.commit()  # Commit both the call summary and relationship
                db.refresh(call_summary)
                print(f"Created call summary: {call_summary.id} for user {user.fullname}")
                print(f"Created relationship: User {user.id} -> Call {call_summary.id}")
                return call_summary
            else:
                db.rollback()
                print(f"No user found with extension number {extension_number}")
                raise ValueError(f"No user found with extension number {extension_number}")
        except Exception as e:
            db.rollback()
            print(f"Error creating call summary: {str(e)}")
            raise e
        finally:
            db.close()

    def get_call_summaries(self,
                          search_field: Optional[str] = None,
                          search_value: Optional[str] = None,
                          start_date: Optional[int] = None,
                          end_date: Optional[int] = None,
                          page: int = 1,
                          per_page: int = 20) -> Dict[str, Any]:
        db = self.get_db()
        try:
            # Print the absolute path to the database file for debugging
            db_url = self.engine.url
            print(f"[DEBUG] Using database at: {db_url}")

            # Print filter parameters for debugging
            print(f"[DEBUG] search_field: {search_field}, search_value: {search_value}, start_date: {start_date}, end_date: {end_date}")

            # Start with a query that includes the relationship
            query = db.query(CallSummary).filter(CallSummary.type == 'external').options(joinedload(CallSummary.users))

            # Debug: count of external calls before further filtering
            try:
                count_external = db.query(CallSummary).filter(CallSummary.type == 'external').count()
                print(f"[DEBUG] Count of external calls: {count_external}")
            except Exception as e:
                print(f"[DEBUG] Error counting external calls: {e}")

            # Apply filters
            if search_field and search_value:
                if search_field == 'all':
                    print(f"[DEBUG] 'all' search: search_value={search_value}")
                    filter_condition = or_(
                        CallSummary.callee.ilike(f'%{search_value}%'),
                        CallSummary.direction.ilike(f'%{search_value}%'),
                        CallSummary.type.ilike(f'%{search_value}%')
                    )
                    print(f"[DEBUG] 'all' search filter: {filter_condition}")
                    query = query.filter(filter_condition)
                    try:
                        debug_count = query.count()
                        print(f"[DEBUG] Results after 'all' filter: {debug_count}")
                    except Exception as e:
                        print(f"[DEBUG] Error counting after 'all' filter: {e}")
                elif search_field == 'callee':
                    query = query.filter(CallSummary.callee.ilike(f'%{search_value}%'))
                elif search_field == 'direction':
                    query = query.filter(CallSummary.direction.ilike(f'%{search_value}%'))
                elif search_field == 'type':
                    query = query.filter(CallSummary.type.ilike(f'%{search_value}%'))

            if start_date not in [None, '']:
                query = query.filter(CallSummary.start_time >= start_date)
            if end_date not in [None, '']:
                query = query.filter(CallSummary.end_time <= end_date)

            # Get total count
            total = query.count()
            print(f"[DEBUG] Total after all filters: {total}")

            # Apply pagination
            query = query.order_by(CallSummary.start_time.desc())
            query = query.offset((page - 1) * per_page).limit(per_page)

            # Get results with user information
            results = query.all()
            print(f"[DEBUG] Results returned: {len(results)}")

            return {
                "total": total,
                "page": page,
                "per_page": per_page,
                "total_pages": (total + per_page - 1) // per_page,
                "results": results
            }
        finally:
            db.close()

    def get_users(
        self,
        search_field: Optional[str] = None,
        search_value: Optional[str] = None,
        page: int = 1,
        per_page: int = 20
    ) -> Dict:
        """Get users with pagination and optional filtering"""
        db = self.get_db()
        try:
            query = db.query(User)

            # Apply search filter if provided
            if search_field and search_value:
                if search_field == 'all':
                    # Search in all supported fields
                    query = query.filter(
                        or_(
                            User.extension_number == str(search_value) if search_value.isdigit() else False,
                            User.fullname.ilike(f'%{search_value}%'),
                            User.role.ilike(f'%{search_value}%')
                        )
                    )
                elif search_field == 'extension_number':
                    query = query.filter(User.extension_number == str(search_value))
                elif search_field == 'fullname':
                    query = query.filter(User.fullname.ilike(f'%{search_value}%'))
                elif search_field == 'role':
                    query = query.filter(User.role.ilike(f'%{search_value}%'))

            # Calculate total count
            total = query.count()

            # Apply pagination
            query = query.order_by(User.id)
            query = query.offset((page - 1) * per_page).limit(per_page)

            # Get results
            users = query.all()

            # Calculate total pages
            total_pages = (total + per_page - 1) // per_page

            return {
                "total": total,
                "page": page,
                "per_page": per_page,
                "total_pages": total_pages,
                "results": users
            }
        except Exception as e:
            db.rollback()
            raise e
        finally:
            db.close() 

    def get_user_by_id(self, user_id: int) -> Optional[User]:
        db = self.get_db()
        try:
            return db.query(User).filter(User.id == user_id).first()
        finally:
            db.close()

    def get_user_call_summaries(
        self,
        user_id: int,
        search_field: Optional[str] = None,
        search_value: Optional[str] = None,
        start_date: Optional[int] = None,
        end_date: Optional[int] = None,
        page: int = 1,
        per_page: int = 20
    ) -> Dict[str, Any]:
        db = self.get_db()
        try:
            db_url = self.engine.url
            print(f"[DEBUG] Using database at: {db_url}")
            print(f"[DEBUG] (user) search_field: {search_field}, search_value: {search_value}, "
                f"start_date: {start_date}, end_date: {end_date}, user_id: {user_id}")

            # Start query: external calls related to the given user_id
            query = (
                db.query(CallSummary)
                .join(RelUserCalls, RelUserCalls.call_summary_id == CallSummary.id)
                .filter(
                    RelUserCalls.user_id == user_id,
                    CallSummary.type == 'external'
                )
                .options(joinedload(CallSummary.users))
            )

            # Debug count
            try:
                count_external = query.count()
                print(f"[DEBUG] (user) Count of external calls for user_id {user_id}: {count_external}")
            except Exception as e:
                print(f"[DEBUG] (user) Error counting external calls: {e}")

            # Apply filters
            if search_field and search_value not in [None, '']:
                if search_field == 'all':
                    filter_condition = or_(
                        CallSummary.extension_number == str(search_value) if search_value.isdigit() else False,
                        CallSummary.callee.ilike(f'%{search_value}%'),
                        CallSummary.direction.ilike(f'%{search_value}%'),
                        CallSummary.type.ilike(f'%{search_value}%')
                    )
                    query = query.filter(filter_condition)
                elif search_field == 'extension_number':
                    query = query.filter(CallSummary.extension_number == str(search_value))
                elif search_field == 'callee':
                    query = query.filter(CallSummary.callee.ilike(f'%{search_value}%'))
                elif search_field == 'direction':
                    query = query.filter(CallSummary.direction.ilike(f'%{search_value}%'))
                elif search_field == 'type':
                    query = query.filter(CallSummary.type.ilike(f'%{search_value}%'))

            if start_date not in [None, '']:
                query = query.filter(CallSummary.start_time >= start_date)
            if end_date not in [None, '']:
                query = query.filter(CallSummary.end_time <= end_date)

            # Get total count
            total = query.count()
            print(f"[DEBUG] (user) Total after all filters: {total}")

            # Apply pagination
            query = query.order_by(CallSummary.start_time.desc())
            query = query.offset((page - 1) * per_page).limit(per_page)

            # Results
            results = query.all()
            print(f"[DEBUG] (user) Results returned: {len(results)}")

            return {
                "total": total,
                "page": page,
                "per_page": per_page,
                "total_pages": (total + per_page - 1) // per_page,
                "results": results
            }
        finally:
            db.close()


    def update_user_by_extension(self, extension_number: str, username: str, fullname: str, role: str = 'CTT user') -> bool:
        db = self.get_db()
        try:
            user = db.query(User).filter(User.extension_number == str(extension_number)).first()
            if user:
                user.fullname = fullname
                user.username = username
                user.role = role
                db.commit()
                print(f"[DEBUG] Updated user with extension {extension_number}: username={username}, fullname={fullname}")
                return True
            else:
                # Create new user
                new_user = User(fullname=fullname, extension_number=extension_number, username=username, role=role)
                db.add(new_user)
                db.commit()
                print(f"[DEBUG] Created new user: extension={extension_number}, username={username}, fullname={fullname}, role={role}")
                return True
        except Exception as e:
            db.rollback()
            print(f"[DEBUG] Error updating/creating user with extension {extension_number}: {e}")
            return False
        finally:
            db.close() 
