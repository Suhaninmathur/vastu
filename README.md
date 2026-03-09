# Vastu Expert API 🕉

REST API for Vastu Shastra house analysis — built with **C++ + Crow framework**.

## Endpoints

| Method | Route | Description |
|--------|-------|-------------|
| GET | `/` | Health check |
| POST | `/api/analyze` | Analyze a single house |
| POST | `/api/compare` | Compare two houses |
| GET | `/api/tip` | Random Vastu tip |

---

## POST /api/analyze

**Request body:**
```json
{
  "plotShape": "Square",
  "entrance":  "NorthEast",
  "kitchen":   "SouthEast",
  "bedroom":   "SouthWest",
  "temple":    "NorthEast"
}
```

Valid values:
- `plotShape` → `Square` | `Rectangle` | `Triangle` | `LShape`
- directions → `North` | `South` | `East` | `West` | `NorthEast` | `NorthWest` | `SouthEast` | `SouthWest`

**Response:**
```json
{
  "totalScore": 100,
  "verdict": "EXCELLENT — Highly Vastu-compliant home.",
  "breakdown": {
    "plot":     { "label": "...", "score": 20, "suggestion": "" },
    "entrance": { "label": "...", "score": 20, "suggestion": "" },
    "kitchen":  { "label": "...", "score": 20, "suggestion": "" },
    "bedroom":  { "label": "...", "score": 20, "suggestion": "" },
    "temple":   { "label": "...", "score": 20, "suggestion": "" }
  }
}
```

---

## POST /api/compare

**Request body:**
```json
{
  "house1": { "plotShape":"Square",    "entrance":"North",    "kitchen":"SouthEast", "bedroom":"SouthWest", "temple":"NorthEast" },
  "house2": { "plotShape":"Rectangle", "entrance":"East",     "kitchen":"NorthWest", "bedroom":"South",     "temple":"East" }
}
```

---

## Deploy on Render

### Step 1 — Push to GitHub
```bash
git init
git add .
git commit -m "initial commit"
git remote add origin https://github.com/YOUR_USERNAME/vastu-api.git
git push -u origin main
```

### Step 2 — Create Web Service on Render
1. Go to https://dashboard.render.com → **New → Web Service**
2. Connect your GitHub repo
3. Set these settings:

| Setting | Value |
|---------|-------|
| **Environment** | `Docker` |
| **Branch** | `main` |
| **Port** | `8080` |

4. Click **Deploy** — Render will build the Docker image and start the service.

### Step 3 — Test your live API
```bash
curl https://your-app.onrender.com/

curl -X POST https://your-app.onrender.com/api/analyze \
  -H "Content-Type: application/json" \
  -d '{"plotShape":"Square","entrance":"NorthEast","kitchen":"SouthEast","bedroom":"SouthWest","temple":"NorthEast"}'
```

---

## Run Locally with Docker

```bash
docker build -t vastu-api .
docker run -p 8080:8080 vastu-api
```

Then open: http://localhost:8080
