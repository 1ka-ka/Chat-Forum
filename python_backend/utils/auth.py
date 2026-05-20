from datetime import datetime, timedelta, timezone
import jwt

JWT_SECRET = "chatforum-secret-key-change-in-production-2025"
JWT_ALGORITHM = "HS256"
JWT_EXPIRATION_HOURS = 24


def create_token(user_id: int) -> str:
    exp = datetime.now(timezone.utc) + timedelta(hours=JWT_EXPIRATION_HOURS)
    payload = {"user_id": user_id, "exp": exp, "iat": datetime.now(timezone.utc)}
    return jwt.encode(payload, JWT_SECRET, algorithm=JWT_ALGORITHM)


def decode_token(token: str) -> int:
    payload = jwt.decode(token, JWT_SECRET, algorithms=[JWT_ALGORITHM])
    return payload["user_id"]
