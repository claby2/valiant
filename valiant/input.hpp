#ifndef VALIANT_INPUT_HPP
#define VALIANT_INPUT_HPP

#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>

#include <string>

#include "error.hpp"

namespace valiant {
enum class EventType { KEYDOWN, KEY, KEYUP };

class Input {
   public:
    SDL_Event event;

    auto get_key_down(const std::string &key) const -> bool {
        return query_key(key, EventType::KEYDOWN);
    }

    auto get_key(const std::string &key) const -> bool {
        return query_key(key, EventType::KEY);
    }

    auto get_key_up(const std::string &key) const -> bool {
        return query_key(key, EventType::KEYUP);
    }

   private:
    auto query_key(std::string key, EventType event_type) const -> bool {
        SDL_Keycode key_code = SDL_GetKeyFromName(key.c_str());
        if (key_code != SDLK_UNKNOWN) {
            bool condition;
            switch (event_type) {
                case EventType::KEYDOWN:
                    condition =
                        (event.type == SDL_KEYDOWN && event.key.repeat == 0);
                    break;
                case EventType::KEY:
                    condition = (event.type == SDL_KEYDOWN);
                    break;
                case EventType::KEYUP:
                    condition = (event.type == SDL_KEYUP &&
                                 event.key.keysym.sym == key_code);
                    break;
                default:
                    throw ValiantError(
                        "Unrecognized event type (INTERNAL ERROR)");
            }
            if (condition && event.key.keysym.sym == key_code) {
                return true;
            }
            return false;
        }
        throw ValiantError("Given key name parameter \"" + key +
                           "\" was not recognized");
        return false;
    }
};
}  // namespace valiant

#endif
