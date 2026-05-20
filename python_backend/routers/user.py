from fastapi import APIRouter, Request, UploadFile, File, Form
from database import get_db_cursor
from utils.file_util import is_valid_image, make_filename, MAX_AVATAR_SIZE
import os

router = APIRouter(prefix="/api/user", tags=["user"])


@router.get("/profile")
async def get_profile(request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    cursor, conn = get_db_cursor()
    try:
        cursor.execute("SELECT id, username, nickname, avatar_url, created_at FROM users WHERE id = %s", (user_id,))
        user = cursor.fetchone()
        if not user:
            return {"code": 404, "message": "用户不存在"}
        user["avatar_url"] = user["avatar_url"] or ""
        return {"code": 200, "message": "success", "data": user}
    finally:
        conn.close()


@router.put("/profile")
async def update_profile(
    request: Request,
    nickname: str = Form(default=""),
    avatar: UploadFile = File(default=None),
):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}

    if nickname and len(nickname) > 50:
        return {"code": 400, "message": "用户名长度不能超过50位字符"}

    avatar_url = ""
    if avatar and avatar.filename:
        if not is_valid_image(avatar.filename):
            return {"code": 400, "message": "不支持的文件格式"}
        filename = make_filename(user_id, avatar.filename)
        upload_dir = "./uploads/avatars"
        os.makedirs(upload_dir, exist_ok=True)
        filepath = os.path.join(upload_dir, filename)
        content = await avatar.read()
        if len(content) > MAX_AVATAR_SIZE:
            return {"code": 400, "message": "头像大小超过2MB限制"}
        with open(filepath, "wb") as f:
            f.write(content)
        avatar_url = f"/uploads/avatars/{filename}"

    cursor, conn = get_db_cursor()
    try:
        if nickname:
            cursor.execute("SELECT id FROM users WHERE nickname = %s AND id != %s", (nickname, user_id))
            if cursor.fetchone():
                return {"code": 409, "message": "该用户名已被使用"}

        if nickname and avatar_url:
            cursor.execute(
                "UPDATE users SET nickname = %s, avatar_url = %s WHERE id = %s",
                (nickname, avatar_url, user_id)
            )
        elif nickname:
            cursor.execute(
                "UPDATE users SET nickname = %s WHERE id = %s",
                (nickname, user_id)
            )
        elif avatar_url:
            cursor.execute("UPDATE users SET avatar_url = %s WHERE id = %s", (avatar_url, user_id))
        else:
            return {"code": 400, "message": "请至少修改一项信息"}
        conn.commit()
        cursor.execute("SELECT id, username, nickname, avatar_url, created_at FROM users WHERE id = %s", (user_id,))
        return {"code": 200, "message": "success", "data": cursor.fetchone()}
    finally:
        conn.close()


@router.get("/{user_id}")
async def get_user_by_id(user_id: int):
    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "SELECT id, nickname, avatar_url, created_at, (SELECT COUNT(*) FROM posts WHERE user_id = u.id) AS post_count FROM users u WHERE u.id = %s",
            (user_id,)
        )
        user = cursor.fetchone()
        if not user:
            return {"code": 404, "message": "用户不存在"}
        user["avatar_url"] = user["avatar_url"] or ""
        return {"code": 200, "message": "success", "data": user}
    finally:
        conn.close()
