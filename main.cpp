#include "crow_all.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

// ════════════════════════════════════════════
//   VASTU EXPERT SYSTEM — Crow REST API
//   Routes:
//     GET  /              health check
//     POST /api/analyze   analyze one house
//     POST /api/compare   compare two houses
//     GET  /api/tip       random Vastu tip
// ════════════════════════════════════════════

// ─── Vastu Logic ─────────────────────────────

struct Result {
    string label;
    int    score;
    string suggestion; // empty = no dosha
};

Result analyzePlot(const string& shape) {
    if (shape == "Square")
        return {"Perfect Vastu Plot — Square gives balanced energy on all 4 sides.", 20, ""};
    if (shape == "Rectangle")
        return {"Good Plot — Rectangle acceptable if length:width ratio < 1:2.", 15, ""};
    if (shape == "Triangle")
        return {"Dosha — Triangular plot causes Agni dosha (disputes/fire).", 3,
                "Plant trees on South & West boundary. Use Vastu pyramids at corners."};
    return {"Dosha — Irregular plot disturbs Pancha Bhuta balance.", 5,
            "Use Vastu pyramids on all corners to neutralise irregular energy."};
}

Result analyzeEntrance(const string& dir) {
    if (dir == "NorthEast")
        return {"Excellent — NorthEast (Ishaan) is the most auspicious entrance.", 20, ""};
    if (dir == "North")
        return {"Very Good — North brings wealth (Kubera direction).", 18, ""};
    if (dir == "East")
        return {"Very Good — East brings health and positive solar energy.", 18, ""};
    if (dir == "West")
        return {"Average — West entrance is neutral.", 10, ""};
    if (dir == "South")
        return {"Dosha — South entrance invites negative energy.", 3,
                "Hang Vastu swastika on South door. Place lead strip below threshold."};
    return {"Moderate — Acceptable if main road faces this direction.", 8, ""};
}

Result analyzeKitchen(const string& dir) {
    if (dir == "SouthEast")
        return {"Perfect — SouthEast is the Agni (fire) corner for kitchen.", 20, ""};
    if (dir == "NorthWest")
        return {"Acceptable — NorthWest kitchen works if stove faces East.", 12, ""};
    if (dir == "NorthEast")
        return {"Dosha — Never place kitchen in NorthEast (destroys prosperity).", 2,
                "Shift cooking to SouthEast. Place red pyramid in kitchen."};
    if (dir == "SouthWest")
        return {"Dosha — SouthWest kitchen causes health issues.", 3,
                "Use orange/red walls. Avoid water source near stove."};
    return {"Moderate — Ensure stove faces East while cooking.", 8, ""};
}

Result analyzeBedroom(const string& dir) {
    if (dir == "SouthWest")
        return {"Perfect — SouthWest gives stability and sound sleep.", 20, ""};
    if (dir == "South")
        return {"Good — South bedroom gives good rest. Head should point South.", 15, ""};
    if (dir == "NorthWest")
        return {"Acceptable — NorthWest is better for guest room.", 10, ""};
    if (dir == "NorthEast")
        return {"Dosha — NorthEast bedroom disturbs mental peace.", 3,
                "Shift master bedroom. Place blue crystal in NorthEast corner."};
    return {"Moderate — Ensure head points South or East while sleeping.", 8, ""};
}

Result analyzeTemple(const string& dir) {
    if (dir == "NorthEast")
        return {"Perfect — NorthEast (Ishaan) is the divine corner for worship.", 20, ""};
    if (dir == "East")
        return {"Good — East is auspicious for pooja facing the rising sun.", 15, ""};
    if (dir == "North")
        return {"Acceptable — North pooja room is fine if NorthEast is unavailable.", 12, ""};
    if (dir == "South")
        return {"Dosha — South temple disturbs prayer energy.", 3,
                "Hang copper pyramid above temple. Face idol East."};
    if (dir == "SouthWest")
        return {"Dosha — SouthWest temple is strongly prohibited.", 1,
                "Relocate temple to NorthEast. Place copper idol facing East."};
    return {"Moderate — Place idol facing East inside the room.", 8, ""};
}

string getVerdict(int score) {
    if (score >= 85) return "EXCELLENT — Highly Vastu-compliant home. Great prosperity ahead!";
    if (score >= 70) return "GOOD — Minor corrections needed for best results.";
    if (score >= 50) return "AVERAGE — Several doshas present. Apply remedies soon.";
    return "POOR — Major Vastu doshas. Structural changes recommended.";
}

const vector<string> TIPS = {
    "Keep the NorthEast corner clean and clutter-free — it is the sacred Ishaan zone.",
    "Never place a mirror directly facing the main entrance — it reflects positive energy out.",
    "Tulsi plant in East or NorthEast brings health and wards off negative energy.",
    "Keep the centre of your home (Brahmasthana) open — avoid toilets or pillars here.",
    "Water elements like aquarium belong in the NorthEast or North direction.",
    "Sleeping with head pointing South gives sound sleep; head towards North is prohibited.",
    "Use yellow or cream colors in the NorthEast zone to amplify divine energy.",
    "Salt in a bowl placed in corners absorbs negative energy — replace every Saturday.",
};

// ─── Main ─────────────────────────────────────

int main() {
    srand((unsigned)time(nullptr));

    crow::SimpleApp app;

    // ── CORS middleware — adds headers to every response ──────────────────────
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .headers("Content-Type", "Authorization")
        .methods("GET"_method, "POST"_method, "OPTIONS"_method)
        .origin("*");   // tighten to your Render frontend URL in production

    // ── GET / — health check ──────────────────────────────────────────────────
    CROW_ROUTE(app, "/")([](){
        crow::json::wvalue res;
        res["status"]  = "ok";
        res["service"] = "Vastu Expert API";
        res["version"] = "1.0";
        return res;
    });

    // ── POST /api/analyze ─────────────────────────────────────────────────────
    // Body: { "plotShape":"Square","entrance":"NorthEast","kitchen":"SouthEast",
    //         "bedroom":"SouthWest","temple":"NorthEast" }
    CROW_ROUTE(app, "/api/analyze").methods("POST"_method)
    ([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body) {
            crow::response res(400);
            res.write("{\"error\":\"Invalid JSON\"}");
            return res;
        }

        // Validate required fields
        for (auto field : {"plotShape","entrance","kitchen","bedroom","temple"}) {
            if (!body.has(field)) {
                crow::response res(400);
                res.write("{\"error\":\"Missing field: " + string(field) + "\"}");
                return res;
            }
        }

        string plotShape = body["plotShape"].s();
        string entrance  = body["entrance"].s();
        string kitchen   = body["kitchen"].s();
        string bedroom   = body["bedroom"].s();
        string temple    = body["temple"].s();

        Result p = analyzePlot(plotShape);
        Result e = analyzeEntrance(entrance);
        Result k = analyzeKitchen(kitchen);
        Result b = analyzeBedroom(bedroom);
        Result t = analyzeTemple(temple);

        int total = p.score + e.score + k.score + b.score + t.score;

        crow::json::wvalue res;
        res["totalScore"] = total;
        res["verdict"]    = getVerdict(total);

        res["breakdown"]["plot"]["label"]      = p.label;
        res["breakdown"]["plot"]["score"]      = p.score;
        res["breakdown"]["plot"]["suggestion"] = p.suggestion;

        res["breakdown"]["entrance"]["label"]      = e.label;
        res["breakdown"]["entrance"]["score"]      = e.score;
        res["breakdown"]["entrance"]["suggestion"] = e.suggestion;

        res["breakdown"]["kitchen"]["label"]      = k.label;
        res["breakdown"]["kitchen"]["score"]      = k.score;
        res["breakdown"]["kitchen"]["suggestion"] = k.suggestion;

        res["breakdown"]["bedroom"]["label"]      = b.label;
        res["breakdown"]["bedroom"]["score"]      = b.score;
        res["breakdown"]["bedroom"]["suggestion"] = b.suggestion;

        res["breakdown"]["temple"]["label"]      = t.label;
        res["breakdown"]["temple"]["score"]      = t.score;
        res["breakdown"]["temple"]["suggestion"] = t.suggestion;

        return crow::response(res);
    });

    // ── POST /api/compare ─────────────────────────────────────────────────────
    // Body: { "house1":{...}, "house2":{...} }
    CROW_ROUTE(app, "/api/compare").methods("POST"_method)
    ([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body || !body.has("house1") || !body.has("house2")) {
            crow::response res(400);
            res.write("{\"error\":\"Provide house1 and house2 objects\"}");
            return res;
        }

        auto score = [](crow::json::rvalue h) -> int {
            return analyzePlot    (h["plotShape"].s()).score
                 + analyzeEntrance(h["entrance"].s()).score
                 + analyzeKitchen (h["kitchen"].s()).score
                 + analyzeBedroom (h["bedroom"].s()).score
                 + analyzeTemple  (h["temple"].s()).score;
        };

        int s1 = score(body["house1"]);
        int s2 = score(body["house2"]);

        string winner;
        if      (s1 > s2) winner = "House 1 has better Vastu alignment.";
        else if (s2 > s1) winner = "House 2 has better Vastu alignment.";
        else              winner = "Both houses have equal Vastu score.";

        crow::json::wvalue res;
        res["house1Score"] = s1;
        res["house2Score"] = s2;
        res["winner"]      = winner;
        return crow::response(res);
    });

    // ── GET /api/tip ──────────────────────────────────────────────────────────
    CROW_ROUTE(app, "/api/tip")([](){
        crow::json::wvalue res;
        res["tip"] = TIPS[rand() % TIPS.size()];
        return res;
    });

    // Render sets PORT env var at runtime
    uint16_t port = 8080;
    const char* env_port = getenv("PORT");
    if (env_port) port = (uint16_t)atoi(env_port);

    app.port(port).multithreaded().run();
    return 0;
}
