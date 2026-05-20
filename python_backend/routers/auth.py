import re
from fastapi import APIRouter, Request
from pydantic import BaseModel
from database import get_db_cursor
from utils.auth import create_token
import bcrypt

router = APIRouter(prefix="/api/auth", tags=["auth"])


class RegisterReq(BaseModel):
    username: str
    password: str
    nickname: str


class LoginReq(BaseModel):
    username: str
    password: str


USERNAME_PATTERN = re.compile(r'^[a-zA-Z0-9_]+$')


@router.post("/register")
def register(req: RegisterReq):
    if len(req.username) < 3 or len(req.username) > 50 or not USERNAME_PATTERN.match(req.username):
        return {"code": 400, "message": "账号格式不正确，需3-50位字母数字下划线"}
    if len(req.password) < 6 or len(req.password) > 50:
        return {"code": 400, "message": "密码长度需6-50位"}
    if not req.nickname or len(req.nickname) > 50:
        return {"code": 400, "message": "用户名格式不正确，需1-50位字符"}

    cursor, conn = get_db_cursor()
    try:
        cursor.execute("SELECT id FROM users WHERE username = %s", (req.username,))
        if cursor.fetchone():
            return {"code": 409, "message": "该账号已存在"}

        cursor.execute("SELECT id FROM users WHERE nickname = %s", (req.nickname,))
        if cursor.fetchone():
            return {"code": 409, "message": "该用户名已被使用"}

        hashed = bcrypt.hashpw(req.password.encode(), bcrypt.gensalt()).decode()
        cursor.execute(
            "INSERT INTO users (username, password_hash, nickname) VALUES (%s, %s, %s)",
            (req.username, hashed, req.nickname)
        )
        conn.commit()
        user_id = cursor.lastrowid
        token = create_token(user_id)

        return {"code": 200, "message": "注册成功", "data": {
            "id": user_id, "username": req.username, "nickname": req.nickname, "token": token
        }}
    finally:
        conn.close()


@router.post("/login")
def login(req: LoginReq):
    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "SELECT id, username, password_hash, nickname, avatar_url FROM users WHERE username = %s",
            (req.username,)
        )
        user = cursor.fetchone()
        if not user or not bcrypt.checkpw(req.password.encode(), user["password_hash"].encode()):
            return {"code": 400, "message": "账号或密码错误"}

        token = create_token(user["id"])
        return {"code": 200, "message": "登录成功", "data": {
            "id": user["id"],
            "username": user["username"],
            "nickname": user["nickname"],
            "avatar_url": user["avatar_url"] or "",
            "token": token
        }}
    finally:
        conn.close()
