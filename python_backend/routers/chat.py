from fastapi import APIRouter, Request, WebSocket, WebSocketDisconnect
from pydantic import BaseModel
from database import get_db_cursor
from utils.auth import decode_token
from utils.notification import create_notification
import json

router = APIRouter(prefix="/api/chat", tags=["chat"])


class ConnectionManager:
    def __init__(self):
        self.active_connections: dict[int, WebSocket] = {}

    async def connect(self, user_id: int, websocket: WebSocket):
        await websocket.accept()
        self.active_connections[user_id] = websocket
        await self.broadcast_online_status(user_id, True)

    def disconnect(self, user_id: int):
        if user_id in self.active_connections:
            del self.active_connections[user_id]

    async def send_to_user(self, user_id: int, data: dict):
        if user_id in self.active_connections:
            try:
                await self.active_connections[user_id].send_json(data)
            except Exception:
                pass

    async def broadcast_online_status(self, user_id: int, online: bool):
        data = {"type": "online_status", "user_id": user_id, "online": online}
        for uid, ws in list(self.active_connections.items()):
            if uid != user_id:
                try:
                    await ws.send_json(data)
                except Exception:
                    pass


manager = ConnectionManager()


async def websocket_chat(websocket: WebSocket, token: str = None):
    if not token:
        await websocket.close(code=4001)
        return

    try:
        user_id = decode_token(token)
    except Exception:
        await websocket.close(code=4001)
        return

    await manager.connect(user_id, websocket)

    try:
        while True:
            raw = await websocket.receive_text()
            try:
                data = json.loads(raw)
            except json.JSONDecodeError:
                continue

            msg_type = data.get("type")

            if msg_type == "ping":
                await websocket.send_json({"type": "pong"})
            elif msg_type == "message":
                receiver_id = data.get("receiver_id")
                content = data.get("content", "").strip()

                if not receiver_id or not content:
                    continue

                cursor, conn = get_db_cursor()
                try:
                    cursor.execute(
                        "INSERT INTO messages (sender_id, receiver_id, content) VALUES (%s, %s, %s)",
                        (user_id, receiver_id, content)
                    )
                    conn.commit()
                    message_id = cursor.lastrowid

                    cursor.execute(
                        "SELECT created_at FROM messages WHERE id = %s",
                        (message_id,)
                    )
                    row = cursor.fetchone()
                    created_at = str(row["created_at"]) if row else ""
                finally:
                    conn.close()

                msg_data = {
                    "type": "message",
                    "message_id": message_id,
                    "sender_id": user_id,
                    "receiver_id": receiver_id,
                    "content": content,
                    "created_at": created_at
                }

                await manager.send_to_user(receiver_id, msg_data)
                await websocket.send_json(msg_data)

                cursor2, conn2 = get_db_cursor()
                try:
                    cursor2.execute("SELECT nickname FROM users WHERE id = %s", (user_id,))
                    sender_row = cursor2.fetchone()
                    sender_nickname = sender_row["nickname"] if sender_row else "用户"
                    create_notification(
                        user_id=receiver_id,
                        actor_id=user_id,
                        ntype="chat",
                        message=f"{sender_nickname} 给你发了私信"
                    )
                finally:
                    conn2.close()
    except WebSocketDisconnect:
        manager.disconnect(user_id)
        await manager.broadcast_online_status(user_id, False)


@router.get("/conversations")
async def get_conversations(request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "SELECT "
            "CASE WHEN m.sender_id = %s THEN m.receiver_id ELSE m.sender_id END AS user_id, "
            "u.nickname, u.avatar_url, m.content AS last_message, m.created_at AS last_time, "
            "(SELECT COUNT(*) FROM messages m2 "
            " WHERE m2.sender_id = CASE WHEN m.sender_id = %s THEN m.receiver_id ELSE m.sender_id END "
            " AND m2.receiver_id = %s AND m2.is_read = 0) AS unread_count "
            "FROM messages m "
            "INNER JOIN ("
            "  SELECT LEAST(sender_id, receiver_id) AS min_id, GREATEST(sender_id, receiver_id) AS max_id, MAX(id) AS max_msg_id "
            "  FROM messages WHERE sender_id = %s OR receiver_id = %s GROUP BY min_id, max_id"
            ") latest ON m.id = latest.max_msg_id "
            "LEFT JOIN users u ON u.id = CASE WHEN m.sender_id = %s THEN m.receiver_id ELSE m.sender_id END "
            "ORDER BY m.created_at DESC",
            (user_id, user_id, user_id, user_id, user_id, user_id)
        )
        items = []
        for row in cursor.fetchall():
            row["avatar_url"] = row["avatar_url"] or ""
            items.append(row)
        return {"code": 200, "message": "success", "data": items}
    finally:
        conn.close()


@router.get("/messages/{other_user_id}")
async def get_messages(other_user_id: int, request: Request, page: int = 1, page_size: int = 20):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    if page < 1:
        page = 1
    if page_size < 1 or page_size > 100:
        page_size = 20
    offset = (page - 1) * page_size

    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "SELECT COUNT(*) AS total FROM messages WHERE (sender_id = %s AND receiver_id = %s) OR (sender_id = %s AND receiver_id = %s)",
            (user_id, other_user_id, other_user_id, user_id)
        )
        total = cursor.fetchone()["total"]
        total_pages = (total + page_size - 1) // page_size

        cursor.execute(
            "SELECT id, sender_id, receiver_id, content, is_read, created_at FROM messages "
            "WHERE (sender_id = %s AND receiver_id = %s) OR (sender_id = %s AND receiver_id = %s) "
            "ORDER BY created_at DESC LIMIT %s OFFSET %s",
            (user_id, other_user_id, other_user_id, user_id, page_size, offset)
        )
        return {"code": 200, "message": "success", "data": {
            "items": cursor.fetchall(), "total": total, "page": page, "page_size": page_size, "total_pages": total_pages
        }}
    finally:
        conn.close()


@router.put("/read/{other_user_id}")
async def mark_as_read(other_user_id: int, request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    cursor, conn = get_db_cursor()
    try:
        cursor.execute("UPDATE messages SET is_read = 1 WHERE sender_id = %s AND receiver_id = %s AND is_read = 0",
                       (other_user_id, user_id))
        conn.commit()
        return {"code": 200, "message": "已标记为已读"}
    finally:
        conn.close()


class SendMessageReq(BaseModel):
    receiver_id: int
    content: str


@router.post("/send")
async def send_message(req: SendMessageReq, request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    if not req.content.strip():
        return {"code": 400, "message": "消息内容不能为空"}

    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "INSERT INTO messages (sender_id, receiver_id, content) VALUES (%s, %s, %s)",
            (user_id, req.receiver_id, req.content.strip())
        )
        conn.commit()
        message_id = cursor.lastrowid

        cursor.execute(
            "SELECT id, sender_id, receiver_id, content, is_read, created_at FROM messages WHERE id = %s",
            (message_id,)
        )
        msg = cursor.fetchone()

        msg_data = {
            "type": "message",
            "message_id": message_id,
            "sender_id": user_id,
            "receiver_id": req.receiver_id,
            "content": req.content.strip(),
            "created_at": str(msg["created_at"]) if msg else ""
        }
        await manager.send_to_user(req.receiver_id, msg_data)

        return {"code": 200, "message": "发送成功", "data": msg}
    finally:
        conn.close()
