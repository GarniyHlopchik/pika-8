#include "button.h"

namespace button {
	void Button::setOnPressCallback(sol::function callback) {
        onPressCallback = callback;
}

    void Button::setOnHoldCallback(sol::function callback) {
		onHoldCallback = callback;
    }

    bool Button::containsCursor(float x, float y) const {
        return x >= _x && x <= _x + _width && y >= _y && y <= _y + _height;
    }

    bool Button::isPressed(InputState state) {
        // Check if button was just pressed this frame
        bool wasPressed = false;

        const double* cursorPos = getRelativeCursorPosition(state);
        if (containsCursor(cursorPos[0], cursorPos[1])) {
            wasPressed = true;
        }

        if (wasPressed && onPressCallback.valid()) {
            try {
                onPressCallback();
            } catch (const sol::error& e) {
                // TODO: Add proper error logging
            }
        }

        return wasPressed;
    }

    bool Button::isDown(InputState state) {
            bool isHeld = false;

            const double* cursorPos = getRelativeCursorPosition(state);
            if (containsCursor(cursorPos[0], cursorPos[1])) {
                    isHeld = true;
            }

            if (isHeld && onHoldCallback.valid()) {
                    try {
                            onHoldCallback();
                    } catch (const sol::error& e) {
                            // TODO: Add proper error logging
                    }
            }

            return isHeld;
    }

	void Button::draw(GFX* gfx) {
		Sprite sprite(_id, nullptr, nullptr, _id, _x, _y, _width, _height, {0.0f, 0.0f, 1.0f, 1.0f}, {255, 255, 255, 255});
		sprite.draw(gfx);
	}
}