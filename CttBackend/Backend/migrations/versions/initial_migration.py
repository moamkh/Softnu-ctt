"""initial migration

Revision ID: initial_migration
Revises: 
Create Date: 2024-03-19 10:00:00.000000

"""
from alembic import op
import sqlalchemy as sa

# revision identifiers, used by Alembic.
revision = 'initial_migration'
down_revision = None
branch_labels = None
depends_on = None

def upgrade():
    # Create users table
    op.create_table('users',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('fullname', sa.String(length=255), nullable=True),
        sa.Column('extension_number', sa.Integer(), nullable=False),
        sa.Column('role', sa.String(length=50), nullable=True),
        sa.PrimaryKeyConstraint('id'),
        sa.UniqueConstraint('extension_number')
    )

    # Create call_summary table
    op.create_table('call_summary',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('start_time', sa.Integer(), nullable=False),
        sa.Column('end_time', sa.Integer(), nullable=False),
        sa.Column('duration', sa.Integer(), nullable=False),
        sa.Column('callee', sa.String(length=50), nullable=False),
        sa.Column('direction', sa.String(length=10), nullable=False),
        sa.Column('type', sa.String(length=20), nullable=True),
        sa.Column('extension_number', sa.Integer(), nullable=False),
        sa.PrimaryKeyConstraint('id')
    )

    # Create rel_user_calls table
    op.create_table('rel_user_calls',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('user_id', sa.Integer(), nullable=False),
        sa.Column('call_summary_id', sa.Integer(), nullable=False),
        sa.ForeignKeyConstraint(['user_id'], ['users.id'], ),
        sa.ForeignKeyConstraint(['call_summary_id'], ['call_summary.id'], ),
        sa.PrimaryKeyConstraint('id')
    )

def downgrade():
    op.drop_table('rel_user_calls')
    op.drop_table('call_summary')
    op.drop_table('users') 