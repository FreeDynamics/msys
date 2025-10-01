# upload packages to prefix.dev
import sys
from pathlib import Path
import hashlib
import requests

channel = sys.argv[1]
token = sys.argv[2]
folder = Path(sys.argv[3])

def upload(fn):
    data = fn.read_bytes()
    # skip if larger than 100Mb
    if len(data) > 500 * 1024 * 1024:
        print("Skipping", fn, "because it is too large")
        return
    name = fn.name
    sha256 = hashlib.sha256(data).hexdigest()
    headers = {
        "X-File-Name": name,
        "X-File-SHA256": sha256,
        "Authorization": f"Bearer {token}",
        "Content-Length": str(len(data) + 1),
        "Content-Type": "application/octet-stream",
    }
    r = requests.post(channel, data=data, headers=headers)
    print(f"Uploaded package {name} with status  {r.status_code}")


if __name__ == "__main__":
    for iterfiles in folder.iterdir():
        if iterfiles.suffix == ".tar.bz2" or iterfiles.suffix == ".conda":
            print("Uploading", iterfiles)
            upload(iterfiles)