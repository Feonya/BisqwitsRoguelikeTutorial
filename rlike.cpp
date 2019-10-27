#include <regex>
#include <random>
#include <algorithm>
#include <ios>
#include <iostream>
#include <iomanip>      // Manipulators for iostream.
#include <memory>
#include <cctype>
#include <deque>
#include <map>
#include <set>

static std::mt19937 rnd;    // Random number engine (Mersenne twister algorithm).

// FRAND() generates a random number between 0 and 1.
#define FRAND()     std::uniform_real_distribution<>(0.f, 1.f)(rnd)
// Generate a random number of specified range.
#define RAND(size)  std::uniform_int_distribution<>(0, (size)-1)(rnd)

/*
       █████████                                  █████      ███    ███
      ███░░░░░███                                ░░███      ███    ░░███
     ███     ░░░   ██████  █████ ████ ████████   ███████   ███      ░░███
    ░███          ███░░███░░███ ░███ ░░███░░███ ░░░███░   ░███       ░███
    ░███         ░███ ░███ ░███ ░███  ░███ ░███   ░███    ░███       ░███
    ░░███     ███░███ ░███ ░███ ░███  ░███ ░███   ░███ ███░░███      ███
     ░░█████████ ░░██████  ░░████████ ████ █████  ░░█████  ░░███    ██░
      ░░░░░░░░░   ░░░░░░    ░░░░░░░░ ░░░░ ░░░░░    ░░░░░    ░░░    ░░░
 */

// Return the size of an array at compile time.
template<typename T, std::size_t size>
constexpr std::size_t Count(const T (&arr)[size]) {
    return size;
}

/*
     ██ ██    ██ ██                       ███    ███
    ░██░██   ░██░██                      ███    ░░███
    ░░ ░░    ░░ ░░            ████████  ███      ░░███
                             ░░███░░███░███       ░███
                              ░███ ░░░ ░███       ░███
                              ░███     ░░███      ███
                    █████████ █████     ░░███    ██░
                   ░░░░░░░░░ ░░░░░       ░░░    ░░░
*/

// Syntactic shorthand for creating regular expressions.
static std::regex operator""_r(const char* pattern, std::size_t length) {
    return std::regex(pattern, length);
}

// English language word manipulations:

/*
     ███████████                                                            █████████              █████     ███           ████             ███    ███
    ░░███░░░░░███                                                          ███░░░░░███            ░░███     ░░░           ░░███            ███    ░░███
     ░███    ░███   ██████  █████████████    ██████  █████ █████  ██████  ░███    ░███  ████████  ███████   ████   ██████  ░███   ██████  ███      ░░███
     ░██████████   ███░░███░░███░░███░░███  ███░░███░░███ ░░███  ███░░███ ░███████████ ░░███░░███░░░███░   ░░███  ███░░███ ░███  ███░░███░███       ░███
     ░███░░░░░███ ░███████  ░███ ░███ ░███ ░███ ░███ ░███  ░███ ░███████  ░███░░░░░███  ░███ ░░░   ░███     ░███ ░███ ░░░  ░███ ░███████ ░███       ░███
     ░███    ░███ ░███░░░   ░███ ░███ ░███ ░███ ░███ ░░███ ███  ░███░░░   ░███    ░███  ░███       ░███ ███ ░███ ░███  ███ ░███ ░███░░░  ░░███      ███
     █████   █████░░██████  █████░███ █████░░██████   ░░█████   ░░██████  █████   █████ █████      ░░█████  █████░░██████  █████░░██████  ░░███    ██░
    ░░░░░   ░░░░░  ░░░░░░  ░░░░░ ░░░ ░░░░░  ░░░░░░     ░░░░░     ░░░░░░  ░░░░░   ░░░░░ ░░░░░        ░░░░░  ░░░░░  ░░░░░░  ░░░░░  ░░░░░░    ░░░    ░░░
 */

static std::string RemoveArticle(const std::string& s) {
    return std::regex_replace(s, "^(?:a|an|the) +"_r, "");
}

/*
     ███████████  ████                                 ████   ███                        ███    ███
    ░░███░░░░░███░░███                                ░░███  ░░░                        ███    ░░███
     ░███    ░███ ░███  █████ ████ ████████   ██████   ░███  ████   █████████  ██████  ███      ░░███
     ░██████████  ░███ ░░███ ░███ ░░███░░███ ░░░░░███  ░███ ░░███  ░█░░░░███  ███░░███░███       ░███
     ░███░░░░░░   ░███  ░███ ░███  ░███ ░░░   ███████  ░███  ░███  ░   ███░  ░███████ ░███       ░███
     ░███         ░███  ░███ ░███  ░███      ███░░███  ░███  ░███    ███░   █░███░░░  ░░███      ███
     █████        █████ ░░████████ █████    ░░████████ █████ █████  █████████░░██████  ░░███    ██░
    ░░░░░        ░░░░░   ░░░░░░░░ ░░░░░      ░░░░░░░░ ░░░░░ ░░░░░  ░░░░░░░░░  ░░░░░░    ░░░    ░░░
 */

static std::string Pluralize(const std::string& s) {
    // Make the name plural by tacking a 's' at the right spot which is usually in the end of the string,
    // but always before any "of" or "make of".

    // Put a placeholder "\001" there first, so we can use it later to decide the correct suffix depending on the word.
    auto temp = std::regex_replace(s, R"(^(.*?)( (?:\(|of\b|made of\b).*)?$)"_r, "$1" "\001" "$2");

    // The correct form of plural suffix depends on how the word ends.
    // This table handles most cases occurring in the game correctly.
    // It is by no means a complete reference for English infection.
    #define ENDS_WITH(s)  std::regex_search(temp,  "(?:" s ")\001"_r)
    #define REPLACE(s, w) std::regex_replace(temp, "(?:" s ")\001"_r, w)
    return ENDS_WITH("s")       ? temp                  :   // leggings, overalls
           ENDS_WITH("y")       ? REPLACE("y",   "ies") :   // berry
           ENDS_WITH("o|sh|ss") ? REPLACE("",    "es")  :   // dish, glass, potato
           ENDS_WITH("ff?")     ? REPLACE("ff?", "ves") :   // staff, wolf
                                  REPLACE("",    "s")   ;   // anything else
    #undef ENDS_WITH
    #undef REPLACE
}

/*
       █████████       █████     █████   █████████              █████     ███           ████             ███    ███
      ███░░░░░███     ░░███     ░░███   ███░░░░░███            ░░███     ░░░           ░░███            ███    ░░███
     ░███    ░███   ███████   ███████  ░███    ░███  ████████  ███████   ████   ██████  ░███   ██████  ███      ░░███
     ░███████████  ███░░███  ███░░███  ░███████████ ░░███░░███░░░███░   ░░███  ███░░███ ░███  ███░░███░███       ░███
     ░███░░░░░███ ░███ ░███ ░███ ░███  ░███░░░░░███  ░███ ░░░   ░███     ░███ ░███ ░░░  ░███ ░███████ ░███       ░███
     ░███    ░███ ░███ ░███ ░███ ░███  ░███    ░███  ░███       ░███ ███ ░███ ░███  ███ ░███ ░███░░░  ░░███      ███
     █████   █████░░████████░░████████ █████   █████ █████      ░░█████  █████░░██████  █████░░██████  ░░███    ██░
    ░░░░░   ░░░░░  ░░░░░░░░  ░░░░░░░░ ░░░░░   ░░░░░ ░░░░░        ░░░░░  ░░░░░  ░░░░░░  ░░░░░  ░░░░░░    ░░░    ░░░
*/

static std::string AddArticle(const std::string& s, bool definite = false) {
    std::string p = RemoveArticle(s);
    if (definite) return "the " + p;
    // Don't add "a" or "an" to a plural form.
    if (p == Pluralize(p)) return p;
    // Add "an" if the word begins with a vowel, "a" otherwise.
    return (std::regex_search(p, "^[aeiou]"_r) ? "an " : "a ") + p;
}

/*
     █████  █████   █████████  ███████████  ███                     █████      ███    ███
    ░░███  ░░███   ███░░░░░███░░███░░░░░░█ ░░░                     ░░███      ███    ░░███
     ░███   ░███  ███     ░░░  ░███   █ ░  ████  ████████   █████  ███████   ███      ░░███
     ░███   ░███ ░███          ░███████   ░░███ ░░███░░███ ███░░  ░░░███░   ░███       ░███
     ░███   ░███ ░███          ░███░░░█    ░███  ░███ ░░░ ░░█████   ░███    ░███       ░███
     ░███   ░███ ░░███     ███ ░███  ░     ░███  ░███      ░░░░███  ░███ ███░░███      ███
     ░░████████   ░░█████████  █████       █████ █████     ██████   ░░█████  ░░███    ██░
      ░░░░░░░░     ░░░░░░░░░  ░░░░░       ░░░░░ ░░░░░     ░░░░░░     ░░░░░    ░░░    ░░░
*/

static std::string UCFirst(const std::string& s) {
    return char(std::toupper(s[0])) + s.substr(1);
}

static const char* const numerals1To12[12 - 1 + 1] = {"one", "two", "three", "four", "five", "six", "seven",
                                                      "eight", "nine", "ten", "eleven", "twelve"};

/*
     █████        ███           █████    █████   ███   █████  ███   █████    █████        █████████                                  █████              ███    ███
    ░░███        ░░░           ░░███    ░░███   ░███  ░░███  ░░░   ░░███    ░░███        ███░░░░░███                                ░░███              ███    ░░███
     ░███        ████   █████  ███████   ░███   ░███   ░███  ████  ███████   ░███████   ███     ░░░   ██████  █████ ████ ████████   ███████    █████  ███      ░░███
     ░███       ░░███  ███░░  ░░░███░    ░███   ░███   ░███ ░░███ ░░░███░    ░███░░███ ░███          ███░░███░░███ ░███ ░░███░░███ ░░░███░    ███░░  ░███       ░███
     ░███        ░███ ░░█████   ░███     ░░███  █████  ███   ░███   ░███     ░███ ░███ ░███         ░███ ░███ ░███ ░███  ░███ ░███   ░███    ░░█████ ░███       ░███
     ░███      █ ░███  ░░░░███  ░███ ███  ░░░█████░█████░    ░███   ░███ ███ ░███ ░███ ░░███     ███░███ ░███ ░███ ░███  ░███ ░███   ░███ ███ ░░░░███░░███      ███
     ███████████ █████ ██████   ░░█████     ░░███ ░░███      █████  ░░█████  ████ █████ ░░█████████ ░░██████  ░░████████ ████ █████  ░░█████  ██████  ░░███    ██░
    ░░░░░░░░░░░ ░░░░░ ░░░░░░     ░░░░░       ░░░   ░░░      ░░░░░    ░░░░░  ░░░░ ░░░░░   ░░░░░░░░░   ░░░░░░    ░░░░░░░░ ░░░░ ░░░░░    ░░░░░  ░░░░░░    ░░░    ░░░
*/

static std::string ListWithCounts(std::deque<std::string>&& items, bool oneLiner = true) {
    // Count the number of times each item occurs.
    std::map<std::string, unsigned> count;
    for (const auto& s : items) ++count[s];
    // Now, deal with each item.
    for (std::size_t a = 0; a < items.size(); ++a) {
        std::string& n = items[a];
        auto i = count.find(n);
        // Was this item one of those duplicated ones?
        if (i->second == 1) continue;
        // Have we already dealt with it?
        if (!i->second) {
            // Yes, delete it.
            items.erase(items.begin() + a--);
            continue;
        }
        // Remove possible indefinite article.
        n = RemoveArticle(n);
        // Add the count. Numbers 2-12 are expressed using an English word.
        if (i->second <= 12) n = numerals1To12[i->second - 1] + std::string(" ") + n;
        else                 n = std::to_string(i->second) + std::string(" ") + n;
        n = Pluralize(n);
        // Remember to not do the same item again.
        i->second = 0;
    }
    // Finally convert the list into text.
    std::string output;
    for (std::size_t a = 0; a < items.size(); ++a)
        if (oneLiner) {
            if (a) output += (a + 1 == items.size()) ? ", and " : ", ";
            output += items[a];
        }
        else output += items[a] + "\n";
    return output;
}

enum { Normal = 64, Bold = 128, ColorMask = 63 };
static const std::map<std::string, unsigned> ansiFeatures = {
    {"dfl",     0},
    {"reset",   37|Normal},
    {"me",      36|Bold},
    {"exit",    33|Bold},
    {"wall",    30|Bold},
    {"road",    33|Normal},
    {"items",   32|Normal},
    {"alert",   31|Bold},
    {"prompt",  37|Bold},
    {"flush",   1}
};

/*
     ███████████
    ░█░░░███░░░█
    ░   ░███  ░   ██████  ████████  █████████████
        ░███     ███░░███░░███░░███░░███░░███░░███
        ░███    ░███████  ░███ ░░░  ░███ ░███ ░███
        ░███    ░███░░░   ░███      ░███ ░███ ░███
        █████   ░░██████  █████     █████░███ █████
       ░░░░░     ░░░░░░  ░░░░░     ░░░░░ ░░░ ░░░░░
 */

/* Support for color terminals. */
struct Term {
    int color = 37;
    bool bold = false, enabled = true;

    std::string format(const std::string& what) {
        static std::regex pat = "`([a-z]+)`|([^`]+|.)"_r;
        std::string result;
        std::smatch res;
        for (auto b=what.begin(), e=what.end(); std::regex_search(b,e,res,pat); b=res[0].second) {
            if (res[2].length()) result += res[2];
            else {
                auto i = ansiFeatures.find(res[1]);
                if (i != ansiFeatures.end())
                    switch (int c = i->second) {
                        case 0: color = 0; break;
                        case 1: std::cout << std::flush; break;
                        default: result += SetColor( c&Bold, c&ColorMask );
                    }
            }
        }

        return result;
    }

    Term& operator<<(const std::string& what) {
        std::cout << format(what);
        return *this;
    }

    std::string SetColor(bool newBold, int newColor) {
        if (((newBold != bold) || newColor != color) && enabled) {
            bold = newBold; color = newColor;
            std::ostringstream oss; oss << "\33[" << (int)bold << ";" << color << "m";
            return oss.str();
        }
        return {};
    }

    void EnableDisable(bool state) {
        enabled = state;
        if (enabled) *this << "`dfl`";
    }
} static term;

/*
     █████  █████                             ███████████               ██████
    ░░███  ░░███                             ░░███░░░░░███             ███░░███
     ░███  ███████    ██████  █████████████   ░███    ░███   ██████   ░███ ░░░   ██████  ████████   ██████  ████████    ██████   ██████
     ░███ ░░░███░    ███░░███░░███░░███░░███  ░██████████   ███░░███ ███████    ███░░███░░███░░███ ███░░███░░███░░███  ███░░███ ███░░███
     ░███   ░███    ░███████  ░███ ░███ ░███  ░███░░░░░███ ░███████ ░░░███░    ░███████  ░███ ░░░ ░███████  ░███ ░███ ░███ ░░░ ░███████
     ░███   ░███ ███░███░░░   ░███ ░███ ░███  ░███    ░███ ░███░░░    ░███     ░███░░░   ░███     ░███░░░   ░███ ░███ ░███  ███░███░░░
     █████  ░░█████ ░░██████  █████░███ █████ █████   █████░░██████   █████    ░░██████  █████    ░░██████  ████ █████░░██████ ░░██████
    ░░░░░    ░░░░░   ░░░░░░  ░░░░░ ░░░ ░░░░░ ░░░░░   ░░░░░  ░░░░░░   ░░░░░      ░░░░░░  ░░░░░      ░░░░░░  ░░░░ ░░░░░  ░░░░░░   ░░░░░░
*/

struct ItemReference {
    // Was this "all" without any specifiers?
    // If so, then refs should contain just 1 element with what = "".
    bool everything = false;

    // Original request.
    std::string original;

    struct SingleReference {
        // What was referred to? blank = everything.
        std::string what;
        // Amount, if given. 0 = 1 shirt, or 1 pile of coins (default)
        //                   1 = 1 shirt, or 1 coin
        //                   2 = 2 shirts, or 2 coins
        long amount = 0;
        // Index, if given. 0 = every item matching the description
        //                  1 = first item matching the description
        //                  2 = second item matching the description
        // Ignored for money.
        long index = 1;
    };
    std::deque<SingleReference> refs, except;

    ItemReference(const char* what) : ItemReference(std::string(what)) {}

    ItemReference(const std::string& what) {
        std::smatch res;
        std::regex_match(what, res, "(.*?)(?: except (.+))?"_r);
        original = res[1];

        // For "all"-type entries, add a dummy entry that indicates "everything".
        if (original == "all" || original == "everything") {
            everything = true;
            SingleReference w;
            w.index = 0;
            refs.push_back(w);
        }
        else if (!original.empty()) {
            // Deal with a comma-separated list of operands.
            ParseReferences(refs, original);
        }

        if (res[2].length()) ParseReferences(except, res[2]);
    }

    // True if this request clearly intends to address only one specific item.
    bool IsSpecific() const {
        return !everything && refs.size() == 1 && refs[0].amount <= 1 && refs[0].index >= 1;
    }

    void ParseReferences(std::deque<SingleReference>& list, const std::string& what) {
        std::regex pat(" *((?:(?! *,| and | *$).)+)(?:[ ,]|and )*");
        std::smatch res;
        for (auto b = what.begin(); std::regex_search(b, what.end(), res, pat); b = res[0].second)
            list.push_back(ParseSingleReference(res[1]));
    }
    SingleReference ParseSingleReference(const std::string& part) const {
        SingleReference w;
        // Read the item count from the begin of the string.
        std::string word = part;
        for (unsigned a = 1; a <= 12; ++a) {
            std::ostringstream oss; oss << "(^" << numerals1To12[a - 1] << "\\b)";
            word = std::regex_replace(word, std::regex(oss.str()), std::to_string(a));
        }
        static std::regex pattern("^((all|[0-9]+) +)? *(.*)");
        std::smatch res;
        std::regex_match(word, res, pattern);
        w.what                = res[3];    // Item name without number.
        std::string numberStr = res[2];    // The number, if any; empty otherwise.
        if (numberStr == "all")      w.index  = 0;
        else if (!numberStr.empty()) w.amount = std::stol(numberStr);
        else {
            // Read the possible item index from the end of the string.
            static std::regex pattern("^(.*?)(?: +([0-9]+))?$");
            std::smatch res;
            std::regex_match(w.what, res, pattern);
            w.what    = res[1];    // Item name without number.
            numberStr = res[2];    // The number, if any; empty otherwise.
            if (!numberStr.empty()) w.index = std::stol(numberStr);
        }
        return w;
    }
};

/*
       █████████                                          ███           ██████████              █████
      ███░░░░░███                                        ░░░           ░░███░░░░███            ░░███
     ███     ░░░   ██████  ████████    ██████  ████████  ████   ██████  ░███   ░░███  ██████   ███████    ██████
    ░███          ███░░███░░███░░███  ███░░███░░███░░███░░███  ███░░███ ░███    ░███ ░░░░░███ ░░░███░    ░░░░░███
    ░███    █████░███████  ░███ ░███ ░███████  ░███ ░░░  ░███ ░███ ░░░  ░███    ░███  ███████   ░███      ███████
    ░░███  ░░███ ░███░░░   ░███ ░███ ░███░░░   ░███      ░███ ░███  ███ ░███    ███  ███░░███   ░███ ███ ███░░███
     ░░█████████ ░░██████  ████ █████░░██████  █████     █████░░██████  ██████████  ░░████████  ░░█████ ░░████████
      ░░░░░░░░░   ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░░     ░░░░░  ░░░░░░  ░░░░░░░░░░    ░░░░░░░░    ░░░░░   ░░░░░░░░
 */

/* First, define some stuff for our adventure. */
struct GenericData {
    // These three generic fields will provide most of our data needs.
    const char* name;
    float worth;
    float weight;
} static const
// Define types of coins. Reference value: 1.0 = gold. Each coin weighs 0.01 units.
moneyTypes[] = {
    {"platinum", 10,   0.01}, {"gold"  , 1,   0.01}, {"silver", 0.6,  0.01},
    {"bronze"  ,  0.4, 0.01}, {"copper", 0.2, 0.01}, {"wood"  , 0.01, 0.01}
},
// Define conditions for items. Reference value: 1.0 = excellent.
// Three most common condition types are listed first.
condTypes[] = {
    {"awesome",  1.2,  0}, {"excellent", 1,   0}, {"good", 0.9, 0},
    {"average",  0.75, 0}, {"poor", 0.5, 0}, {"bad",  0.6, 0},
    {"thrashed", 0.4,  0}
},
// Define building materials. The raw material cost is included, and the weight.
// Tow most common build types are listed first.
buildTypes[] = {
    {"iron",     0.4,  3  }, {"fur",    0.01, 0.2}, {"gold",     1,    3.5},
    {"bronze",   0.1,  2.7}, {"pewter", 0.05, 2  }, {"chromium", 0.9,  2  },
    {"platinum", 2,    4  }, {"bamboo", 0.01, 1  }, {"leather",  0.09, 0.5},
    {"silk",     0.03, 0.1}, {"steel",  0.7,  3  }, {"glass",    0.04, 2  }
},
// List of all possible treasures. Each treasure has a "type", a "build material" and a "condition",
// which together define the item's worth in money. Additionally, the player may occasionally find raw money.
// The worth multiplier and weight multiplier are provided.
// Reference values: shirt = 1.0 worth, 1.0 weight.
// Four most common item types are listed first.
// Most of these are armour items, because our material list
// includes both hard materials (metals etc) and soft materials (silk etc).
itemTypes[] = {
    {"shirt",       1,    1   }, {"shoe",    0.4, 1  }, {"bracelet", 0.2, 0.2},
    {"tie",         0.25, 0.25}, {"sceptre", 4,   2.5}, {"crown",    3,   0.6},
    {"leggings",    0.8,  0.5 }, {"dagger",  0.1, 1.5}, {"cap",      0.6, 0.5},
    {"battlesuit", 10,    5.0 }, {"hammer",  0.4, 3.0}, {"cape",     0.7, 1  },
    {"overalls",    4,    4.0 }
},
// List of different kinds of tunnels. It is just for variance.
envTypes[] = {
    {"dark"      , 0, 0}, {"tall"  , 0, 0}, {"humid", 0, 0},
    {"beautifull", 0, 0}, {"narrow", 0, 0}
},
// Finally, a list of achievement.
// They are crypted with a reversible cipher to prevent spoiling the game
// to a person who happens to glance over the source code.
foodTypes[] = {
    {"b akbdl epqfts dblf",           50000, 0}, {"b kbqhf okbsf pe dgjdlfm kfht", 35000, 0},
    {"b dbvkcqpm pe dpplfc opsbspft", 20000, 0}, {"b dgjdlfm gps cph",             10000, 0},
    {"dgfftf bmc nbdbqpmj",            6000, 0}, {"b avssfqnjkl ajtdvjs",           3000, 0},
    {"b apjkfc fhh",                   2000, 0}, {"tpnf kjdgfm tsfx",               1000, 0},
    {"b xppc dpqsfw tbmcxjdg",          700, 0}, {"b dvo pe ujmfhbq",                500, 0},
    {"b dvo pe bookf tffct",            300, 0}, {"b qpssfm dbqqps",                 200, 0},
    {"b nvh pe nvccz xbsfq",            110, 0}, {"tpnf qbaajs cqpoojmht",            70, 0},
    {"b cfbc dpdlqpbdg",                 50, 0}, {"b npmsg pkc tojcfq xfa",           30, 0},
    {"b gjkk pe cvts",                   16, 0}, {"b gfbo pe cvts",                    8, 0},
    {"b ajh ojkf pe cvts",                4, 0}, {"b ojkf pe cvts",                    2, 0},
    {"b tofdlkf pe cvts",                 1, 0}
}; // key: badcfehgjilknmporqtsvuxwzy

/*
       █████████                                            ███                     ███    ███
      ███░░░░░███                                          ░░░                     ███    ░░███
     ░███    ░███  ████████  ████████  ████████   ██████   ████   █████   ██████  ███      ░░███
     ░███████████ ░░███░░███░░███░░███░░███░░███ ░░░░░███ ░░███  ███░░   ███░░███░███       ░███
     ░███░░░░░███  ░███ ░███ ░███ ░███ ░███ ░░░   ███████  ░███ ░░█████ ░███████ ░███       ░███
     ░███    ░███  ░███ ░███ ░███ ░███ ░███      ███░░███  ░███  ░░░░███░███░░░  ░░███      ███
     █████   █████ ░███████  ░███████  █████    ░░████████ █████ ██████ ░░██████  ░░███    ██░
    ░░░░░   ░░░░░  ░███░░░   ░███░░░  ░░░░░      ░░░░░░░░ ░░░░░ ░░░░░░   ░░░░░░    ░░░    ░░░
                   ░███      ░███
                   █████     █████
                  ░░░░░     ░░░░░
*/

// Determine how well the player character could eat by selling all their treasures.
static std::string Appraise(double value, int v = 1, std::size_t maxi = 3) {
    std::deque<std::string> list; redo:
    for (const auto& f : foodTypes)
        if (value >= f.worth) {
            std::string k = f.name;
            for (auto& c : k) if (c - ' ') c = 1 + ((c - 1) ^ v);
            list.push_back(k); value -= f.worth;
            if (list.size() < maxi) goto redo;
            break;
        }
    if (list.empty()) return "nothing at all";
    return ListWithCounts(std::move(list));
}

/*
     █████  █████                             ███████████
    ░░███  ░░███                             ░█░░░███░░░█
     ░███  ███████    ██████  █████████████  ░   ░███  ░  █████ ████ ████████   ██████
     ░███ ░░░███░    ███░░███░░███░░███░░███     ░███    ░░███ ░███ ░░███░░███ ███░░███
     ░███   ░███    ░███████  ░███ ░███ ░███     ░███     ░███ ░███  ░███ ░███░███████
     ░███   ░███ ███░███░░░   ░███ ░███ ░███     ░███     ░███ ░███  ░███ ░███░███░░░
     █████  ░░█████ ░░██████  █████░███ █████    █████    ░░███████  ░███████ ░░██████
    ░░░░░    ░░░░░   ░░░░░░  ░░░░░ ░░░ ░░░░░    ░░░░░      ░░░░░███  ░███░░░   ░░░░░░
                                                           ███ ░███  ░███
                                                          ░░██████   █████
                                                           ░░░░░░   ░░░░░
 */

struct ItemType {
    // Any item has these three attributes.
    std::size_t type      = FRAND() > 0.4 ? RAND(Count(itemTypes))  : RAND(4);
    std::size_t build     = FRAND() > 0.4 ? RAND(Count(buildTypes)) : RAND(2);
    std::size_t condition = FRAND() > 0.8 ? RAND(Count(condTypes))  : RAND(3);

    std::string GetType() const;
    std::string GetMaterial() const;
    std::string GetCondition() const;

    // There are various ways that an item can be called.
    //      mat=1:      changes "shirt" into "silk shirt"
    //      mat=2:      changes "shirt" into "shirt made of silk"
    //      cond=1:     changes "shirt" into "awesome shirt"
    std::string Name(int cond = 0, int mat = 0) const;
    std::string Look_(bool specific) const;

    float Weight() const {
        return buildTypes[build].weight * itemTypes[type].weight;
    }
    float Value(float constant = 300.0f) const {
        return constant * buildTypes[build].worth * itemTypes[type].worth * condTypes[condition].worth;
    }
};

/*
     ██████████
    ░░███░░░░░█
     ░███  █ ░   ████████
     ░██████    ███░░███
     ░███░░█   ░███ ░███
     ░███ ░   █░███ ░███
     ██████████░░███████
    ░░░░░░░░░░  ░░░░░███
                    ░███
                    █████
                   ░░░░░
 */

// Collection of items and money, either in character's pocket, on the ground, or in a container.
struct Eq {
    std::deque<ItemType> items;
    long money[Count(moneyTypes)] = { 0 };

    // Calculate the total worth of all these items and coins.
    float Value() const {
        // Count all the money.
        float result = 0.0f; size_t a = 0;
        for (auto m : money)        result += m * moneyTypes[a++].worth;
        // Add the worth of all items.
        for (const auto& i : items) result += i.Value();
        return result;
    }
    // Calculate the total weight of all these items and coins.
    float Weight() const {
        // Count all the money.
        float result = 0.0f; size_t a = 0;
        for (auto m : money)        result += m * moneyTypes[a++].weight;
        // Add the weight of all items.
        for (const auto& i : items) result += i.Weight();
        return result;
    }
    long Burden() const {
        return 1 + Weight();
    }
    std::size_t CountItems() const {
        std::size_t result = items.size();
        for (auto l : money) if (l) ++result;
        return result;
    }

    // Clear the list of items (or generate N random items).
    void Clear(std::size_t n = 0) {
        items.resize(n);
        for (auto& m : money) m = 0;
    }

    // Generate the output for "looking at" an item.
    //      n   = Which item to look at
    //      in  = true, if the player wants to look "in" the item
    //      all = true, if this is not the only item being listed at once
    std::pair<std::string, float> LookItem(std::size_t n, bool specific) const {
        const auto& item = items[n];
        return {item.Look_(specific), item.Value()};
    }

    // Generate the output for "looking at" money.
    std::pair<std::string, float> LookMoney(long m, bool specific) const {
        std::ostringstream oss; oss << money[m] << moneyTypes[m].name << (money[m] == 1 ? "coin" : "coins");
        std::string info, common = oss.str();
        if (specific) {
            std::ostringstream oss;
            oss << "The coins are worth " << std::fixed << std::setprecision(2)
                << money[m] * moneyTypes[m].worth << " gold total.\n";
            info = oss.str();
        }
        return {common + info, money[m] * moneyTypes[m].worth};
    }

    // Generate the output for checking out the whole inventory.
    //      isInv         = false if this is not player's inventory.
    //      retval.second = false if the inventory is empty.
    std::pair<std::string, bool> Print(bool isInv) const {
        float itemsValue = 0.0f, moneyValue = 0.0f;
        std::string result;

        // List all items and count their total value.
        std::deque<std::string> names;
        for (const auto& i : items) {
            names.push_back(AddArticle(i.Name(0, 1)));
            itemsValue += i.Value();
        }

        result += ListWithCounts(std::move(names), false);

        if (isInv && itemsValue != 0.0f) {
            std::ostringstream oss; oss << "The total value of your items is " << std::fixed << std::setprecision(2)
                                        << itemsValue << " gold.\n";
            result += oss.str();
        }

        // Also report the total weight of everything.
        if (isInv) {
            std::ostringstream oss;
            oss << "Your possessions wear you down " << Burden() << " points for every step you take.\n"
                << "You estimate that these possessions could earn you " << Appraise(Value()) << "\n";
            result += oss.str();
        }

        return {result, moneyValue != 0.0f || !items.empty()};
    }

    // Finds money matching the given keywords. -1 = no money found
    // We accept the following types of expressions:
    //      gold        -- matches gold coins
    //      gold coin   -- matches gold coins
    //      gold coins  -- matches gold coins
    //      coins       -- matches all coins
    //      blank/all   -- matches all coins
    // Ignores "amount" and "index" in the SingleReference.
    long FindMoney(const ItemReference::SingleReference& w, std::size_t first = 0) const {
        // For each type of coins that does exist, accept it, if it matches the user's request.
        for (std::size_t m = first; m < Count(moneyTypes); ++m) {
            std::ostringstream oss;
            oss << "|money|coins?|" << moneyTypes[m].name << "( coins?)?";
            if (money[m] > 0 && std::regex_match(w.what, std::regex(oss.str()))) return m;
        }
        return -1;
    }
    // Finds items matching the given keywords. -1 = no item found
    // We accept the following type of expressions:
    //      an awesome gold sceptre -- matches awesome gold sceptre
    //      awesome gold sceptre    -- matches awesome gold sceptre
    //      a gold sceptre          -- matches any gold sceptre
    //      gold sceptre            -- matches any gold sceptre
    //      an awesome sceptre      -- matches any awesome sceptre
    //      awesome sceptre         -- matches any awesome sceptre
    //      a sceptre               -- matches any sceptre
    //      sceptre                 -- matches any sceptre
    //      sceptres                -- matches any sceptre
    //      shirt 2                 -- only matches the second shirt
    //      blank/all               -- matches anything
    // Ignores "amount" in the SingleReference
    long FindItem(const ItemReference::SingleReference& w, std::size_t first = 0) const {
        // From more specific to less specific, check if we found what the player asked for.
        long occurrences = 0;
        for (std::size_t a = 0; a < items.size(); ++a)
            for (int level = 3 * 2 * 4 - 1; level >= 0; --level) {
                std::string n = items[a].Name((level / 3) % 2, level % 3);
                if (level / 6 == 1) n = AddArticle(n, false);
                if (level / 6 == 2) n = AddArticle(n, true);
                if (level / 6 == 3) n = Pluralize(n);
                if (w.what.empty() || w.what == n) {
                    // break = continue item loop.
                    if (w.index && !w.amount && ++occurrences != w.index) break;
                    if (a < first) break;
                    return a;
                }
            }
        // Give up if nothing matched.
        return -1;
    }
} static eq;

std::string ItemType::GetType() const {
    return itemTypes[type].name;
}

std::string ItemType::GetMaterial() const {
    return buildTypes[build].name;
}

std::string ItemType::GetCondition() const {
    return condTypes[condition].name;
}

std::string ItemType::Name(int cond, int mat) const {
    std::string result = GetType(), m = GetMaterial();
    // Special hack: If the material is "wood", use "wooden" instead.
    if (mat  == 1) result = (m == "wood" ? "wooden" : m) + " " + result;
    if (cond == 1) result = GetCondition() + " " + result;
    if (mat  == 2) result += " made of " + m;
    return result;
}

std::string ItemType::Look_(bool specific) const {
    std::string info, common = specific ? "It is " + AddArticle(Name(0, 2)) + ". It is in " + GetCondition() + " condition.\n"
                                        : "You see " + AddArticle(Name(0, 2)) + ", in " + GetCondition() + " condition.\n";

    if (specific) info = "You estimate that with it you could probably purchase " + Appraise(Value(), 1, 1) + ".\n";

    return common + info;
}

/*
     ███████████
    ░░███░░░░░███
     ░███    ░███   ██████   ██████  █████████████
     ░██████████   ███░░███ ███░░███░░███░░███░░███
     ░███░░░░░███ ░███ ░███░███ ░███ ░███ ░███ ░███
     ░███    ░███ ░███ ░███░███ ░███ ░███ ░███ ░███
     █████   █████░░██████ ░░██████  █████░███ █████
    ░░░░░   ░░░░░  ░░░░░░   ░░░░░░  ░░░░░ ░░░ ░░░░░
 */

// Any particular room in the puzzle may contain the following (Every cell is a room):
struct Room {
    std::size_t wall = 0, env = 0;      // Indexs.
    unsigned seed = 0;                  // For maze generation.
    Eq items;                           // What is lying on the floor.
// Create a model "default" room based on empty definitions.
} static const defaultRoom;


/*
     ██████   ██████
    ░░██████ ██████
     ░███░█████░███   ██████    █████████  ██████
     ░███░░███ ░███  ░░░░░███  ░█░░░░███  ███░░███
     ░███ ░░░  ░███   ███████  ░   ███░  ░███████
     ░███      ░███  ███░░███    ███░   █░███░░░
     █████     █████░░████████  █████████░░██████
    ░░░░░     ░░░░░  ░░░░░░░░  ░░░░░░░░░  ░░░░░░
 */

struct Maze {
    // A maze contains rooms.
    std::map<long/*x*/, std::map<long/*y*/, Room>> rooms;

    // Generate a room at given coordinates.
    // The "model" room will help the maze generator generate similar rooms in nearby locations.
    Room& GenerateRoom(long x, long y, const Room& model, unsigned seed) {
        // Reinitalizes the internal state of the random-number engine using new seed value.
        rnd.seed( y*0xC70F6907UL + x*2166136261UL );
        // Function insert returns a pair:
        // first object is a pointer point to the insertion(also a pair).
        // second object is a bool value, true on sucesss, false on failure.
        auto insRes = rooms[x].insert( {y, model} );
        Room& room = insRes.first->second;
        if (insRes.second) {
            room.items.Clear();
            // If a new room was indeed inserted, make changes in it.
            room.seed = (seed + (FRAND() > 0.95 ? RAND(4) : 0)) & 3;
            // 10% chance for the environment type to change.
            if (FRAND() > 0.9) room.env = RAND(Count(envTypes));
            if (FRAND() > (seed == model.seed ? 0.95 : 0.1)) room.wall = FRAND() < 0.4 ? 2 : 0;
            // Generate a few items in the room.
            room.items.Clear(unsigned(std::pow(FRAND(), 40.0) * 8.5));
        }
        return room;
    }
    // Describe the room with a single character.
    char Char(long x, long y) const {
        // Function find returns a pointer point to the target pair:
        // The pair's first object equals the parameter passed to the find funcion.
        // The second object is the value we would like to get.
        auto i = rooms.find(x);     if (i == rooms.end())     return ' ';
        auto j = i->second.find(y); if (j == i->second.end()) return ' ';
        if (j->second.wall)                 return '#';
        if (!j->second.items.items.empty()) return 'i';
        return '.';
    }
} static maze;

// Player's location and life.
static long x = 0, y = 0, life = 1000;

/*
       █████████                       ██████   ██████                               ███████████            ███    ███
      ███░░░░░███                     ░░██████ ██████                               ░█░░░███░░░█           ███    ░░███
     ███     ░░░   ██████   ████████   ░███░█████░███   ██████  █████ █████  ██████ ░   ░███  ░   ██████  ███      ░░███
    ░███          ░░░░░███ ░░███░░███  ░███░░███ ░███  ███░░███░░███ ░░███  ███░░███    ░███     ███░░███░███       ░███
    ░███           ███████  ░███ ░███  ░███ ░░░  ░███ ░███ ░███ ░███  ░███ ░███████     ░███    ░███ ░███░███       ░███
    ░░███     ███ ███░░███  ░███ ░███  ░███      ░███ ░███ ░███ ░░███ ███  ░███░░░      ░███    ░███ ░███░░███      ███
     ░░█████████ ░░████████ ████ █████ █████     █████░░██████   ░░█████   ░░██████     █████   ░░██████  ░░███    ██░
      ░░░░░░░░░   ░░░░░░░░ ░░░░ ░░░░░ ░░░░░     ░░░░░  ░░░░░░     ░░░░░     ░░░░░░     ░░░░░     ░░░░░░    ░░░    ░░░
 */

static bool CanMoveTo(long whereX, long whereY, const Room& model = defaultRoom) {
    if (!maze.GenerateRoom(whereX, whereY, model, 0).wall) return true;
    return false;
}

/*
      █████████                                                 ███████████                                               ███    ███
     ███░░░░░███                                               ░░███░░░░░███                                             ███    ░░███
    ░███    ░░░  ████████   ██████   █████ ███ █████ ████████   ░███    ░███   ██████   ██████  █████████████    █████  ███      ░░███
    ░░█████████ ░░███░░███ ░░░░░███ ░░███ ░███░░███ ░░███░░███  ░██████████   ███░░███ ███░░███░░███░░███░░███  ███░░  ░███       ░███
     ░░░░░░░░███ ░███ ░███  ███████  ░███ ░███ ░███  ░███ ░███  ░███░░░░░███ ░███ ░███░███ ░███ ░███ ░███ ░███ ░░█████ ░███       ░███
     ███    ░███ ░███ ░███ ███░░███  ░░███████████   ░███ ░███  ░███    ░███ ░███ ░███░███ ░███ ░███ ░███ ░███  ░░░░███░░███      ███
    ░░█████████  ░███████ ░░████████  ░░████░████    ████ █████ █████   █████░░██████ ░░██████  █████░███ █████ ██████  ░░███    ██░
     ░░░░░░░░░   ░███░░░   ░░░░░░░░    ░░░░ ░░░░    ░░░░ ░░░░░ ░░░░░   ░░░░░  ░░░░░░   ░░░░░░  ░░░░░ ░░░ ░░░░░ ░░░░░░    ░░░    ░░░
                 ░███
                 █████
                ░░░░░
 */

static Room& SpawnRooms(long whereX, long whereY, const Room& model = defaultRoom) {
    Room& room = maze.GenerateRoom(whereX, whereY, model, 0);
    // Use this algorithm to generate 4 rooms around (x,y),
    // the seeds of these rooms are 1, 2, 3, 4 respectively.
    #define SPAWN_4_ROOMS(x, y) \
        for (char p : {1, 3, 5, 7}) \
            maze.GenerateRoom(x + p%3-1, y + p/3-1, room, (p+1)/2)
    SPAWN_4_ROOMS(whereX, whereY);
    for (int o=1; o<5 && CanMoveTo(whereX, whereY+o, room); ++o) SPAWN_4_ROOMS(whereX, whereY+o);
    for (int o=1; o<5 && CanMoveTo(whereX, whereY-o, room); ++o) SPAWN_4_ROOMS(whereX, whereY-o);
    for (int o=1; o<6 && CanMoveTo(whereX+o, whereY, room); ++o) SPAWN_4_ROOMS(whereX+o, whereY);
    for (int o=1; o<6 && CanMoveTo(whereX-o, whereY, room); ++o) SPAWN_4_ROOMS(whereX-o, whereY);
    return room;
}



/*
     █████                         █████        ███    ███
    ░░███                         ░░███        ███    ░░███
     ░███         ██████   ██████  ░███ █████ ███      ░░███
     ░███        ███░░███ ███░░███ ░███░░███ ░███       ░███
     ░███       ░███ ░███░███ ░███ ░██████░  ░███       ░███
     ░███      █░███ ░███░███ ░███ ░███░░███ ░░███      ███
     ███████████░░██████ ░░██████  ████ █████ ░░███    ██░
    ░░░░░░░░░░░  ░░░░░░   ░░░░░░  ░░░░ ░░░░░   ░░░    ░░░
 */

// This routine is responsible for providing the view for the player.
// It also generates new maze data.
static void Look() {
    // Generate rooms in the field of vision of the player.
    const Room& room = SpawnRooms(x, y);

    // Generate the current map view.
    std::vector<std::string> mapGraph;
    for (long yo=-4; yo<=4; ++yo) {
        std::string line;
        static const std::map<char, const char*> translation = {
            {'@', "`me`"},
            {'#', "`wall`"},
            {'.', "`road`"},
            {'i', "`items`"}
        };
        for (long xo=-5; xo<=5; ++xo) {
            char c = ((xo==0 && yo==0) ? '@' : maze.Char(x+xo, y+yo));
            auto i = translation.find(c);
            if (i != translation.end()) line += i->second;
            line += c;
        }
        std::ostringstream oss; oss << "`dfl`" << line << "`reset`";
        mapGraph.push_back(oss.str());
    }

    // This is the text that will be printed on the right side of the map.
    std::ostringstream oss;
    oss << "`reset`In a "  << envTypes[room.env].name << " tunnel at " << std::setw(3) << x << "," << std::setw(3) << -y << "\n"
        << "`reset`Exits:`exit`" << (CanMoveTo(x,y-1) ? " north" : "")
                                 << (CanMoveTo(x,y+1) ? " south" : "")
                                 << (CanMoveTo(x-1,y) ? " west"  : "")
                                 << (CanMoveTo(x+1,y) ? " east"  : "") << "\n\n";
    const std::string infoStr = oss.str() + room.items.Print(false).first;

    // Print the map and the information side by side.
    auto m = mapGraph.begin();
    auto b = infoStr.begin(), e = infoStr.end();
    auto pat = "([^\n]*)\n"_r;  // There're two regexs: "[^\n]*\n" and "[^\n]*".
    for (std::smatch res; m!=mapGraph.end() || b!=e; res=std::smatch{}) {
        // res[0] matchs "[^\n]*\n", res[1] matchs "[^\n]*".
        // In res[0]: first object means the start of matching sequence.
        //            second object means the end of matching sequence (The start of res[1]).
        if (b != e) { std::regex_search(b, e, res, pat); b = res[0].second; }
        std::string sa = m!=mapGraph.end() ? *m++ : std::string(11, ' ');
        std::string sb = res.ready() ? (std::string)res[1] : "";
        term << "`dfl`" << sa << " | " << "`items`" << sb << "\n";
    }
}

/*
     ██████████            █████    █████        ███     ██████             ███    ███
    ░░███░░░░░█           ░░███    ░░███        ░░░     ███░░███           ███    ░░███
     ░███  █ ░   ██████   ███████   ░███        ████   ░███ ░░░   ██████  ███      ░░███
     ░██████    ░░░░░███ ░░░███░    ░███       ░░███  ███████    ███░░███░███       ░███
     ░███░░█     ███████   ░███     ░███        ░███ ░░░███░    ░███████ ░███       ░███
     ░███ ░   █ ███░░███   ░███ ███ ░███      █ ░███   ░███     ░███░░░  ░░███      ███
     ██████████░░████████  ░░█████  ███████████ █████  █████    ░░██████  ░░███    ██░
    ░░░░░░░░░░  ░░░░░░░░    ░░░░░  ░░░░░░░░░░░ ░░░░░  ░░░░░      ░░░░░░    ░░░    ░░░
 */

static void EatLife(long l) {
    const char* msg = nullptr;
    if (life>=800 && life-l<800) msg = "You are so hungry!\n";
    if (life>=150 && life-l<150) msg = "You are famished!\n";
    if (life>=70  && life-l<70)  msg = "You are about to collapse any second!\n";
    life -= l;
    if (msg) { term << "`alert`" << msg << "`reset`"; }
}

/*
     ███████████                      ██████   ██████                               ███████████               ███    ███
    ░█░░░███░░░█                     ░░██████ ██████                               ░░███░░░░░███             ███    ░░███
    ░   ░███  ░  ████████  █████ ████ ░███░█████░███   ██████  █████ █████  ██████  ░███    ░███ █████ ████ ███      ░░███
        ░███    ░░███░░███░░███ ░███  ░███░░███ ░███  ███░░███░░███ ░░███  ███░░███ ░██████████ ░░███ ░███ ░███       ░███
        ░███     ░███ ░░░  ░███ ░███  ░███ ░░░  ░███ ░███ ░███ ░███  ░███ ░███████  ░███░░░░░███ ░███ ░███ ░███       ░███
        ░███     ░███      ░███ ░███  ░███      ░███ ░███ ░███ ░░███ ███  ░███░░░   ░███    ░███ ░███ ░███ ░░███      ███
        █████    █████     ░░███████  █████     █████░░██████   ░░█████   ░░██████  ███████████  ░░███████  ░░███    ██░
       ░░░░░    ░░░░░       ░░░░░███ ░░░░░     ░░░░░  ░░░░░░     ░░░░░     ░░░░░░  ░░░░░░░░░░░    ░░░░░███   ░░░    ░░░
                            ███ ░███                                                              ███ ░███
                           ░░██████                                                              ░░██████
                            ░░░░░░                                                                ░░░░░░
 */

static bool TryMoveBy(int xd, int yd) {
    // If we are moving diagonally, ensure that there is an actual path.
    if (!CanMoveTo(x+xd, y+yd) || (!CanMoveTo(x, y+yd) && !CanMoveTo(x+xd, y)))
        { term << "You cannot go that way.\n"; return false; }

    long burden = 1;
    x += xd; y += yd;
    EatLife(burden);

    return true;
}

/*
     █████                         █████        █████████    █████      ███    ███
    ░░███                         ░░███        ███░░░░░███  ░░███      ███    ░░███
     ░███         ██████   ██████  ░███ █████ ░███    ░███  ███████   ███      ░░███
     ░███        ███░░███ ███░░███ ░███░░███  ░███████████ ░░░███░   ░███       ░███
     ░███       ░███ ░███░███ ░███ ░██████░   ░███░░░░░███   ░███    ░███       ░███
     ░███      █░███ ░███░███ ░███ ░███░░███  ░███    ░███   ░███ ███░░███      ███
     ███████████░░██████ ░░██████  ████ █████ █████   █████  ░░█████  ░░███    ██░
    ░░░░░░░░░░░  ░░░░░░   ░░░░░░  ░░░░ ░░░░░ ░░░░░   ░░░░░    ░░░░░    ░░░    ░░░
*/

static void LookAt(const ItemReference& what, const ItemReference&) {
    const Room& room = maze.GenerateRoom(x, y, defaultRoom, 0);
    const Eq& where  = room.items;

    std::string hereStr = "here";

    // Look at items in the room.
    for (const auto& w : what.refs) {
        std::pair<std::string, float> output;
        auto AddItems = [&output](const std::pair<std::string, float>& src) {
            output.first  += src.first;
            output.second += src.second;
        };

        for (long no = 0; (no = where.FindItem(w, no)) >= 0; ) {
            AddItems(where.LookItem(no++, what.IsSpecific()));
            if (what.IsSpecific()) break;
        }

        // Look at money in the room, if there were no items or we're looking at everything.
        if (what.everything || output.first.empty())
            for (long no = 0; (no = where.FindMoney(w, no)) >= 0; ) {
                AddItems(where.LookMoney(no++, what.IsSpecific()));
                if (what.IsSpecific()) break;
            }

        bool roomEmpty = output.first.empty();

        // Look at inventory items, if there was nothing particular in the room.
        if (output.first.empty())
            for (long no = 0; (no = eq.FindMoney(w, no)) >= 0; ) {
                AddItems(eq.LookMoney(no++, what.IsSpecific()));
                if (what.IsSpecific()) break;
            }

        // Look at inventory money, if ...
        if (output.first.empty() || (what.everything && roomEmpty))
            for (long no = 0; (no = eq.FindMoney(w, no)) >= 0; ) {
                AddItems(eq.LookMoney(no++, what.IsSpecific()));
                if (what.IsSpecific()) break;
            }

        if (!what.IsSpecific() && !output.first.empty()) {
            if (output.second < 1.0f)
                output.first += "It is of no sales value at all.\n";
            else
                output.first += "You estimate that with them you could probably buy " +
                                 Appraise(output.second, 1, 1) + ".\n";
        }

        if (!output.first.empty()) term << output.first;
        else
            if (what.IsSpecific())
                term << "There " << (w.what.back() == 's' ? "are" : "is") << " no "
                     << w.what << " " << hereStr << " that you can look at.\n";
            else
                term << "There is nothing " << hereStr + ".\n";
    }
}

/*
       █████████   ████   ███
      ███░░░░░███ ░░███  ░░░
     ░███    ░███  ░███  ████   ██████    █████
     ░███████████  ░███ ░░███  ░░░░░███  ███░░
     ░███░░░░░███  ░███  ░███   ███████ ░░█████
     ░███    ░███  ░███  ░███  ███░░███  ░░░░███
     █████   █████ █████ █████░░████████ ██████
    ░░░░░   ░░░░░ ░░░░░ ░░░░░  ░░░░░░░░ ░░░░░░
 */

struct Alias {
    std::regex  pattern;
    std::string replacement;
} static const aliases[] = {
    { R"(^l\b)"_r,                      "look"            },
    { R"(^lat? )"_r,                    "look at "        },
    { R"(^lin? )"_r,                    "look in "        },
    { R"(^look in )"_r,                 "look at all in " },
    { R"(^ga\b)"_r,                     "get all"         },
    { R"(^da\b)"_r,                     "drop all"        },
    { R"(^d )"_r,                       "drop "           },
    { R"(^g )"_r,                       "get "            },
    { R"(^take )"_r,                    "get "            },
    { R"(^pry )"_r,                     "open "           },
    { R"(^i\b)"_r,                      "inv"             },
    { R"(^inventory\b)"_r,              "inv"             },
    { R"(^da\b)"_r,                     "drop all"        },
    { R"(^put(.*)\b(in|into|to)\b)"_r,  "drop$1in"        },
    { R"(\busing\b)"_r,                 "with"            },
    { R"(\bwith my\b)"_r,               "with"            },
    { R"(^\s+)"_r,                      ""                },
    { R"(\s+$)"_r,                      ""                },
};

/*
       █████████                                                                    █████ ███████████                          █████
      ███░░░░░███                                                                  ░░███ ░░███░░░░░███                        ░░███
     ███     ░░░   ██████  █████████████   █████████████    ██████   ████████    ███████  ░███    ░███   ██████   ██████    ███████   ██████  ████████
    ░███          ███░░███░░███░░███░░███ ░░███░░███░░███  ░░░░░███ ░░███░░███  ███░░███  ░██████████   ███░░███ ░░░░░███  ███░░███  ███░░███░░███░░███
    ░███         ░███ ░███ ░███ ░███ ░███  ░███ ░███ ░███   ███████  ░███ ░███ ░███ ░███  ░███░░░░░███ ░███████   ███████ ░███ ░███ ░███████  ░███ ░░░
    ░░███     ███░███ ░███ ░███ ░███ ░███  ░███ ░███ ░███  ███░░███  ░███ ░███ ░███ ░███  ░███    ░███ ░███░░░   ███░░███ ░███ ░███ ░███░░░   ░███
     ░░█████████ ░░██████  █████░███ █████ █████░███ █████░░████████ ████ █████░░████████ █████   █████░░██████ ░░████████░░████████░░██████  █████
      ░░░░░░░░░   ░░░░░░  ░░░░░ ░░░ ░░░░░ ░░░░░ ░░░ ░░░░░  ░░░░░░░░ ░░░░ ░░░░░  ░░░░░░░░ ░░░░░   ░░░░░  ░░░░░░   ░░░░░░░░  ░░░░░░░░  ░░░░░░  ░░░░░
 */

// A command line history and input engine.
struct CommandReader {
    enum : unsigned { HistLen = 10, HistMin = 5 };

    std::deque<std::string> history;
    std::string prompt;
    std::pair<std::string, unsigned> repeat;

    void SetPrompt(const std::string& s) { prompt = s; }

    std::string ReadCommand() {
        for (;;) {
            term << "`prompt`" << prompt << "`reset``flush`";

            std::string cmd;

            if (repeat.second) { --repeat.second; cmd=repeat.first; }
            else {
                // Wait for user input.
                std::getline(std::cin, cmd);
                if (!std::cin.good()) return "quit";
            }
            if (cmd.empty()) continue;

            // Check if the command begins with a number, indicating
            // a desire to repeat a command a number of times.
            std::smatch res;
            if (std::regex_match(cmd, res, "^([1-9][0-9]*) +([^ 1-9].*)"_r)) {
                repeat = { res[2], std::stoi(res[1]) };
                if (repeat.second > 50) {
                    term << "Ignoring too large repeat count " << std::to_string(repeat.second) << "\n";
                    repeat.second = 0;
                }
                continue;
            }

            // Add every command to the history.
            if (cmd[0]!='!' && !repeat.second && cmd.size()>=HistMin) {
                history.push_back(cmd);
                if (history.size() > HistLen) history.pop_front();
            }

            // Deal with history searches.
            if (cmd[0]=='!' && cmd !="!?")
                for (std::size_t a = history.size(); a-- > 0; ) {
                    if (history[a].compare(0, cmd.size()-1, cmd, 1, cmd.size()-1) == 0) {
                        term << "Repeating " << history[a] << "\n";
                        cmd = history[a];
                        break;
                    }
                if (cmd[0] == '!') term << "No match found for " << cmd.substr(1) << " from command history.\n";
                if (cmd[0] == '!') continue;
            }

            // Apply command aliases after dealing with the history.
            for (;;) {
                std::string origCmd = cmd;
                for (const auto& r : aliases)
                    cmd = std::regex_replace(cmd, r.pattern, r.replacement);
                if (cmd == origCmd) break;
            }
            return cmd;
        }
    }

    void PrintHistory() {
        // Produce out the history of commands:
        std::ostringstream oss; oss << "`reset`Your latest commands of at least " << int(HistMin) << " characters:\n";
        term << oss.str();
        for (std::size_t a = 0; a < history.size(); ++a) {
            std::ostringstream oss; oss << std::setw(3) << a + 1 << " : " << history[a] << "\n";
            term << oss.str();
        }
    }
};

/*
                                ███               ███    ███
                               ░░░               ███    ░░███
     █████████████    ██████   ████  ████████   ███      ░░███
    ░░███░░███░░███  ░░░░░███ ░░███ ░░███░░███ ░███       ░███
     ░███ ░███ ░███   ███████  ░███  ░███ ░███ ░███       ░███
     ░███ ░███ ░███  ███░░███  ░███  ░███ ░███ ░░███      ███
     █████░███ █████░░████████ █████ ████ █████ ░░███    ██░
    ░░░░░ ░░░ ░░░░░  ░░░░░░░░ ░░░░░ ░░░░ ░░░░░   ░░░    ░░░
 */

int main() {
    term << "`reset`Welcome to the treasure dungeon.\n";

    CommandReader cmd;

help:
    term << "`reset`Available commands:\n"
            "\tl/look\n"
            "\tla/look at <item>\n"
            "\tn/s/w/e for moving\n"
            "\tansi off, if the colors don't work for you\n"
            "\tquit\n"
            "\thelp\n\n"
            "You are starving. You are trying to find enough stuff to sell\n"
            "for food before you die. Beware, food is very expensive here.\n\n";


    // The main loop.
    Look();
    while (life > 0) {
        std::ostringstream oss; oss << "[life:" << life << "]> ";
        cmd.SetPrompt(oss.str());

        auto s = cmd.ReadCommand();
        // Produce the prompt and wait for player's command.
        if (s == "quit") break;
        if (s.empty()) continue;

        // Parse the command using C++11 regex.
        std::smatch res;

        #define RM std::regex_match

        // First, some metacommands.
        if (s == "!?" || s == "history") cmd.PrintHistory();
        else if (RM(s, R"((?:help|what|\?))"_r)) goto help;

        // Some fundamental movement commands.
        else if (RM(s, "((go|walk|move) +)?(n|north)"_r))      { if (TryMoveBy( 0,-1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(s|south)"_r))      { if (TryMoveBy( 0, 1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(w|west)"_r))       { if (TryMoveBy(-1, 0)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(e|east)"_r))       { if (TryMoveBy( 1, 0)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(nw|northwest)"_r)) { if (TryMoveBy(-1,-1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(ne|northeast)"_r)) { if (TryMoveBy( 1,-1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(sw|southwest)"_r)) { if (TryMoveBy(-1, 1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(se|southeast)"_r)) { if (TryMoveBy( 1, 1)) Look(); }

        // Then commands for looking at things.
        // Use the power of regex to recognize complex syntax.
        else if (RM(s, "look( +around)?"_r)) Look();
        else if (RM(s, res, "look(?: +at)? +(.*?)(?: +in +(.+))?"_r)) LookAt(res[1].str(), res[2].str());

        else if (RM(s, res, "ansi +(off|on)"_r)) term.EnableDisable(res[1] == "on");
        else if (RM(s, R"((?:wear|wield|eq)\b.*)"_r))
            term << "You are scavenging for survival and not playing an RPG character.\n";
        else if (RM(s, R"(eat\b.*)"_r))
            term << "You have nothing edible! You are hoping to collect something you can sell for food.\n";

        // Any unrecognized command.
        else term << "what\n";
    }

    term << "`alert`" << (life < 0 ? "You are pulled out from the maze by a supernatural force!\n" : "byebye\n")
         << "[life:" << std::to_string(life) << "] Game over\n`reset`";
}