#define CROW_USE_BOOST 0
#define ASIO_STANDALONE
#include "crow_all.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Result {
    string label;
    int    score;
    string suggestion;
};

Result analyzePlot(const string& s) {
    if (s == "Square")    return {"Perfect Vastu Plot - Square gives balanced energy.", 20, ""};
    if (s == "Rectangle") return {"Good Plot - Rectangle acceptable if ratio < 1:2.", 15, ""};
    if (s == "Triangle")  return {"Dosha - Triangular plot causes Agni dosha.", 3, "Plant trees on South & West. Use Vastu pyramids at corners."};
    return {"Dosha - Irregular plot disturbs Pancha Bhuta balance.", 5, "Use Vastu pyramids on all corners."};
}

Result analyzeEntrance(const string& d) {
    if (d == "NorthEast") return {"Excellent - NorthEast (Ishaan) is most auspicious.", 20, ""};
    if (d == "North")     return {"Very Good - North brings wealth (Kubera direction).", 18, ""};
    if (d == "East")      return {"Very Good - East brings health and solar energy.", 18, ""};
    if (d == "West")      return {"Average - West entrance is neutral.", 10, ""};
    if (d == "South")     return {"Dosha - South entrance invites negative energy.", 3, "Hang Vastu swastika on South door."};
    return {"Moderate - Acceptable if main road faces this direction.", 8, ""};
}

Result analyzeKitchen(const string& d) {
    if (d == "SouthEast") return {"Perfect - SouthEast is the Agni fire corner.", 20, ""};
    if (d == "NorthWest") return {"Acceptable - NorthWest works if stove faces East.", 12, ""};
    if (d == "NorthEast") return {"Dosha - Kitchen in NorthEast destroys prosperity.", 2, "Shift cooking to SouthEast."};
    if (d == "SouthWest") return {"Dosha - SouthWest kitchen causes health issues.", 3, "Use orange/red walls."};
    return {"Moderate - Ensure stove faces East while cooking.", 8, ""};
}

Result analyzeBedroom(const string& d) {
    if (d == "SouthWest") return {"Perfect - SouthWest gives stability and sound sleep.", 20, ""};
    if (d == "South")     return {"Good - South bedroom gives good rest.", 15, ""};
    if (d == "NorthWest") return {"Acceptable - NorthWest better for guest room.", 10, ""};
    if (d == "NorthEast") return {"Dosha - NorthEast bedroom disturbs mental peace.", 3, "Shift master bedroom."};
    return {"Moderate - Ensure head points South or East while sleeping.", 8, ""};
}

Result analyzeTemple(const string& d) {
    if (d == "NorthEast") return {"Perfect - NorthEast (Ishaan) is the divine corner.", 20, ""};
    if (d == "East")      return {"Good - East is auspicious for pooja.", 15, ""};
    if (d == "North")     return {"Acceptable - North is fine if NorthEast unavailable.", 12, ""};
    if (d == "South")     return {"Dosha - South temple disturbs prayer energy.", 3, "Face idol East."};
    if (d == "SouthWest") return {"Dosha - SouthWest temple is strongly prohibited.", 1, "Relocate to NorthEast."};
    return {"Moderate - Place idol facing East inside the room.", 8, ""};
}

string getVerdict(int score) {
    if (score >= 85) return "EXCELLENT - Highly Vastu-compliant home!";
    if (score >= 70) return "GOOD - Minor corrections needed.";
    if (score >= 50) return "AVERAGE - Several doshas present. Apply remedies.";
    return "POOR - Major Vastu doshas. Corrections recommended.";
}

const vector<string> TIPS = {
    "Keep the NorthEast corner clean - it is the sacred Ishaan zone.",
    "Never place a mirror directly facing the main entrance.",
    "Tulsi plant in East or NorthEast brings health and positive energy.",
    "Keep the centre (Brahmasthana) open - avoid toilets or pillars here.",
    "Water elements like aquarium belong in NorthEast or North.",
    "Sleeping with head pointing South gives sound sleep.",
    "Use yellow or cream colors in the NorthEast zone.",
    "Salt in a bowl in corners absorbs negative energy - replace every Saturday.",
};

void addCORS(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin",  "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    srand((unsigned)time(nullptr));
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        crow::json::wvalue r;
        r["status"]  = "ok";
        r["service"] = "Vastu Expert API";
        return r;
    });

    CROW_ROUTE(app, "/api/analyze").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        addCORS(res);
        auto body = crow::json::load(req.body);
        if (!body) { res.code=400; res.write("{\"error\":\"Invalid JSON\"}"); res.end(); return; }

        for (auto f : {"plotShape","entrance","kitchen","bedroom","temple"}) {
            if (!body.has(f)) { res.code=400; res.write("{\"error\":\"Missing field\"}"); res.end(); return; }
        }

        auto p = analyzePlot    (body["plotShape"].s());
        auto e = analyzeEntrance(body["entrance"].s());
        auto k = analyzeKitchen (body["kitchen"].s());
        auto b = analyzeBedroom (body["bedroom"].s());
        auto t = analyzeTemple  (body["temple"].s());
        int total = p.score + e.score + k.score + b.score + t.score;

        crow::json::wvalue r;
        r["totalScore"] = total;
        r["verdict"]    = getVerdict(total);
        r["breakdown"]["plot"]["label"]          = p.label;
        r["breakdown"]["plot"]["score"]          = p.score;
        r["breakdown"]["plot"]["suggestion"]     = p.suggestion;
        r["breakdown"]["entrance"]["label"]      = e.label;
        r["breakdown"]["entrance"]["score"]      = e.score;
        r["breakdown"]["entrance"]["suggestion"] = e.suggestion;
        r["breakdown"]["kitchen"]["label"]       = k.label;
        r["breakdown"]["kitchen"]["score"]       = k.score;
        r["breakdown"]["kitchen"]["suggestion"]  = k.suggestion;
        r["breakdown"]["bedroom"]["label"]       = b.label;
        r["breakdown"]["bedroom"]["score"]       = b.score;
        r["breakdown"]["bedroom"]["suggestion"]  = b.suggestion;
        r["breakdown"]["temple"]["label"]        = t.label;
        r["breakdown"]["temple"]["score"]        = t.score;
        r["breakdown"]["temple"]["suggestion"]   = t.suggestion;
        res.write(r.dump()); res.end();
    });

    CROW_ROUTE(app, "/api/compare").methods("POST"_method)
    ([](const crow::request& req, crow::response& res){
        addCORS(res);
        auto body = crow::json::load(req.body);
        if (!body || !body.has("house1") || !body.has("house2")) {
            res.code=400; res.write("{\"error\":\"Provide house1 and house2\"}"); res.end(); return;
        }
        auto score = [](crow::json::rvalue h) {
            return analyzePlot    (h["plotShape"].s()).score
                 + analyzeEntrance(h["entrance"].s()).score
                 + analyzeKitchen (h["kitchen"].s()).score
                 + analyzeBedroom (h["bedroom"].s()).score
                 + analyzeTemple  (h["temple"].s()).score;
        };
        int s1 = score(body["house1"]);
        int s2 = score(body["house2"]);
        string winner = s1>s2 ? "House 1 has better Vastu alignment."
                      : s2>s1 ? "House 2 has better Vastu alignment."
                      : "Both houses have equal Vastu score.";
        crow::json::wvalue r;
        r["house1Score"] = s1;
        r["house2Score"] = s2;
        r["winner"]      = winner;
        res.write(r.dump()); res.end();
    });

    CROW_ROUTE(app, "/api/tip")([](){
        crow::json::wvalue r;
        r["tip"] = TIPS[rand() % TIPS.size()];
        return r;
    });

    uint16_t port = 8080;
    if (const char* p = getenv("PORT")) port = (uint16_t)atoi(p);
    app.port(port).multithreaded().run();
    return 0;
}
