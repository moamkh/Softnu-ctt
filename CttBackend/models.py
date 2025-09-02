from sqlalchemy import Column, Integer, String, DateTime, Float, ForeignKey, Text, Boolean, UniqueConstraint
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.sql import func
from sqlalchemy.orm import relationship

Base = declarative_base()

class User(Base):
    __tablename__ = 'users'
    id = Column(Integer, primary_key=True)
    fullname = Column(String(255), unique=True, nullable=True)
    extension_number = Column(String(50), unique=True, nullable=False)
    username = Column(String(50), unique=True, nullable=True)
    is_active = Column(Boolean, default=True)
    # Relationship with call summaries
    call_summaries = relationship("CallSummary", secondary="rel_user_calls", back_populates="users")

class CallSummary(Base):
    __tablename__ = 'call_summary'
    id = Column(Integer, primary_key=True)
    start_time = Column(Integer, nullable=False)
    end_time = Column(Integer, nullable=False)
    duration = Column(Integer, nullable=False)
    callee = Column(String(50), nullable=False)
    direction = Column(String(10), nullable=False)
    type = Column(String(20), nullable=True)
    extension_number = Column(String(50), nullable=False)
    status = Column(String(255), nullable=True)
    ai_response = Column(Text, nullable=True)
    unique_id = Column(String(255), nullable=True)
    created_at = Column(DateTime, server_default=func.now(), nullable=True)
    # Relationship with users
    users = relationship("User", secondary="rel_user_calls", back_populates="call_summaries")

class RelUserCalls(Base):
    __tablename__ = 'rel_user_calls'
    id = Column(Integer, primary_key=True)
    user_id = Column(Integer, ForeignKey('users.id'), nullable=False)
    call_summary_id = Column(Integer, ForeignKey('call_summary.id', ondelete='CASCADE'), nullable=False) 