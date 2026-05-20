from fastapi import Request, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from utils.auth import decode_token

security = HTTPBearer()


async def get_current_user(request: Request) -> int:
    auth: HTTPAuthorizationCredentials | None = None
    try:
        auth = await security(request)
    except Exception:
        pass
    if auth:
        try:
            return decode_token(auth.credentials)
        except Exception:
            pass
    return 0
