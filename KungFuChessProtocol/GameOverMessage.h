#pragma once

#include <string>

// {"type":"gameOver","result":"white","whiteNewRating":1216,"blackNewRating":1184}
// - sent once, broadcast to every connected client, the moment a game
// finishes. "result" is one of "white", "black", "draw".
//
// The rating fields are optional: if either player's identity wasn't
// known (e.g. an anonymous connection that never logged in), there is no
// rating update to report, and build()/parse() omit them entirely -
// {"type":"gameOver","result":"white"}. This is the ONE place that shape
// is defined - both build() (used by GameEndCoordinator, server-side) and
// parse() (used client-side) go through here, so the two sides can't
// silently drift apart the way independently hand-written build/parse
// code could.
namespace GameOverMessage
{
    // Pass hasRatings=false to omit the rating fields (whiteNewRating/
    // blackNewRating are ignored in that case).
    std::string build(const std::string& result, bool hasRatings, int whiteNewRating, int blackNewRating);

    // Returns false (leaving every out-param untouched) if this isn't a
    // valid gameOver message. outHasRatings reports whether both rating
    // fields were present; when false, outWhiteNewRating/outBlackNewRating
    // are left untouched.
    bool parse(const std::string& rawMessage, std::string& outResult, bool& outHasRatings, int& outWhiteNewRating, int& outBlackNewRating);
}
