from fastapi import FastAPI, Query, HTTPException, Depends, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse
from typing import Optional, List, Dict, Any
from database import DatabaseController
from datetime import datetime
from pydantic import BaseModel
import traceback
from fastapi import status
import re
import json
from sqlalchemy import func

app = FastAPI()
db = DatabaseController()

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.middleware("http")
async def catch_exceptions_middleware(request: Request, call_next):
    try:
        return await call_next(request)
    except Exception as e:
        print(f"Error processing request: {str(e)}")
        print(traceback.format_exc())
        return JSONResponse(
            status_code=500,
            content={"detail": str(e), "traceback": traceback.format_exc()}
        )

@app.get("/")
async def root():
    return {
        "name": "NovinAmiVoip API",
        "version": "1.0.0",
        "endpoints": {
            "users": "/api/users",
            "call_summaries": "/api/call-summaries"
        }
    }

# Models
class UserBase(BaseModel):
    fullname: Optional[str] = None
    extension_number: str
    role: str = "CTT user"
    username: Optional[str] = None

class UserCreate(UserBase):
    pass

class User(UserBase):
    id: int
    last_active: Optional[int] = None
    is_active: Optional[bool] = True
    class Config:
        from_attributes = True

class UserList(BaseModel):
    total: int
    page: int
    per_page: int
    total_pages: int
    results: List[User]
    class Config:
        from_attributes = True

class CallSummaryBase(BaseModel):
    start_time: int
    end_time: int
    duration: int
    callee: str
    direction: str
    type: str
    extension_number: str

class CallSummary(CallSummaryBase):
    id: int
    status: Optional[str] = None
    ai_response: Optional[str] = None
    unique_id: Optional[str] = None
    created_at: Optional[datetime] = None
    class Config:
        from_attributes = True

class UserCallSummaries(BaseModel):
    user: User
    calls: List[CallSummary]
    total: int
    page: int
    per_page: int
    total_pages: int
    class Config:
        from_attributes = True

def paginate_results(results, page, per_page):
    total = len(results)
    total_pages = max(1, (total + per_page - 1) // per_page)
    start = (page - 1) * per_page
    end = start + per_page
    paginated = results[start:end]
    return paginated, total, total_pages

# Endpoints
@app.post("/api/users", response_model=User)
async def create_user(user: UserCreate):
    try:
        return db.create_user(
            fullname=user.fullname,
            extension_number=user.extension_number,
            username=user.username,
            role=user.role
        )
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/api/users/{user_id}", response_model=User)
async def get_user_by_id(user_id: int):
    user = db.get_user_by_id(user_id)
    if not user:
        raise HTTPException(status_code=404, detail="User not found")
    return user

@app.get("/api/users/{user_id}/call-summaries", response_model=UserCallSummaries)
async def get_user_call_summaries(
    user_id: int,
    search_field: Optional[str] = Query(None),
    search_value: Optional[str] = Query(None),
    start_date: Optional[int] = Query(None),
    end_date: Optional[int] = Query(None),
    page: int = Query(1, ge=1),
    per_page: int = Query(20, ge=1, le=100)
):
    try:
        user = db.get_user_by_id(user_id)
        if not user:
            raise HTTPException(status_code=404, detail="User not found")
        if search_field and search_field not in ['callee', 'all']:
            raise HTTPException(status_code=400, detail="Only 'callee' or 'all' is supported")
        result = db.get_user_call_summaries(
            user_id=user_id,
            search_field=search_field,
            search_value=search_value,
            start_date=start_date,
            end_date=end_date,
            page=page,
            per_page=per_page
        )
        calls = result["results"]
        for call in calls:
            if hasattr(call, 'users'):
                delattr(call, 'users')
        return {
            "user": user,
            "calls": calls,
            "total": result["total"],
            "page": result["page"],
            "per_page": result["per_page"],
            "total_pages": result["total_pages"]
        }
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/api/users", response_model=UserList)
async def list_users(
    search_field: Optional[str] = Query(None),
    search_value: Optional[str] = Query(None),
    page: int = Query(1, ge=1),
    per_page: int = Query(20, ge=1, le=100)
):
    try:
        result = db.get_users(
            search_field=search_field,
            search_value=search_value,
            page=page,
            per_page=per_page
        )
        users = result["results"]
        if users:
            extension_numbers = [user.extension_number for user in users]
            from database import CallSummary
            db_session = db.get_db()
            last_calls = db_session.query(
                CallSummary.extension_number,
                func.max(CallSummary.end_time).label("last_active")
            ).filter(
                CallSummary.type == 'external',
                CallSummary.extension_number.in_(extension_numbers)
            ).group_by(CallSummary.extension_number).all()
            db_session.close()
            last_active_map = {ext: last_active for ext, last_active in last_calls}
            for user in users:
                user.last_active = last_active_map.get(user.extension_number, None)
        return {
            "total": result["total"],
            "page": result["page"],
            "per_page": result["per_page"],
            "total_pages": result["total_pages"],
            "results": users
        }
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/api/call-summaries")
async def list_call_summaries(
    search_field: Optional[str] = Query(None),
    search_value: Optional[str] = Query(None),
    start_date: Optional[int] = Query(None),
    end_date: Optional[int] = Query(None),
    page: int = Query(1, ge=1),
    per_page: int = Query(20, ge=1, le=100)
):
    try:
        result = db.get_call_summaries(
            search_field=search_field,
            search_value=search_value,
            start_date=start_date,
            end_date=end_date,
            page=page,
            per_page=per_page
        )
        return result
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/api/call-summaries/{call_id}", response_model=CallSummary)
async def get_call_summary(call_id: int):
    try:
        call_summary = db.get_call_summary_by_id(call_id)
        if not call_summary:
            raise HTTPException(status_code=404, detail="Call summary not found")
        return call_summary
    except Exception as e:
        raise HTTPException(status_code=400, detail=str(e))

@app.get("/api/users/{user_id}/call-summaries/analysis/{call_summary_id}")
async def analyze_call_summary(user_id: int, call_summary_id: int):
    call_summary = db.get_call_summary_by_id(call_summary_id)
    if not call_summary:
        return JSONResponse(status_code=404, content={"status": False, "message": "Call summary not found"})
    user_match = any(user.id == user_id for user in getattr(call_summary, 'users', []))
    if not user_match:
        return JSONResponse(status_code=400, content={"status": False, "message": "The call summary id doesn't belong to the user"})

    ai_response = getattr(call_summary, 'ai_response', None)
    if ai_response and isinstance(ai_response, str):
        try:
            ai_response = json.loads(ai_response)
        except:
            pass

    if isinstance(ai_response, dict) and 'rating' in ai_response and 'summary' in ai_response:
        rating_text = ai_response['rating']
        summary_text = ai_response['summary']
        try:
            if '<summary>' in summary_text and '</summary>' not in summary_text:
                summary_text += '</summary>'

            json_match = re.search(r'<json>(.*?)</json>', rating_text, re.DOTALL)
            codeblock_match = re.search(r'```json\s*(.*?)\s*```', rating_text, re.DOTALL)

            ratings = {}
            if json_match:
                ratings = json.loads(json_match.group(1))
            elif codeblock_match:
                ratings = json.loads(codeblock_match.group(1))
            else:
                # No rating JSON block found; use default empty values
                ratings = {
                    "overall_rating": 0,
                    "dimension_scores": {},
                    "strengths": None,
                    "areas_for_improvement": None,
                    "exceptional_moments": None
                }

            overall_rating = ratings.get("overall_rating", 0)
            if not isinstance(overall_rating, (int, float)):
                ds = ratings.get("dimension_scores", {})
                values = [ds.get(k, 0) for k in ['responsiveness', 'problem_solving', 'communication_clarity', 'empathy', 'efficiency']]
                if all(isinstance(v, (int, float)) for v in values):
                    overall_rating = sum(values) / len(values)
                else:
                    overall_rating = 0

            summary_match = re.search(r'<summary>(.*?)</summary>', summary_text, re.DOTALL)
            summary = summary_match.group(1).strip() if summary_match else re.sub(r'</?summary>', '', summary_text).strip()

            return JSONResponse(status_code=200, content={
                "status": True,
                "data": {
                    "client_phonenumber": call_summary.callee,
                    "summary": summary,
                    "overal_rating": overall_rating,
                    "dimension_scores": ratings.get('dimension_scores', {}),
                    "strengths": ratings.get('strengths'),
                    "areas_for_improvement": ratings.get('areas_for_improvement'),
                    "exceptional_moments": ratings.get('exceptional_moments')
                },
                "message": "AI analysis completed successfully"
            })
        except Exception as e:
            return JSONResponse(status_code=200, content={"status": False, "ai_response": ai_response, "message": f"Failed to parse AI response: {str(e)}"})
    else:
        return JSONResponse(status_code=200, content={"status": False, "ai_response": ai_response, "message": "Something went wrong with the AI analysis."})
