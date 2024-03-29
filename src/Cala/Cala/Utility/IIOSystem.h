#pragma once
#include <glm/glm.hpp>
#include <bitset>

struct GLFWwindow;

namespace Cala {
	class IIOSystem {
	public:
		IIOSystem();
		~IIOSystem();
		virtual void update();

		enum KeyCode {
			MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE,	// Mouse codes

			KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G,	// Alpha
			KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N,	// Characters
			KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U,	// Key
			KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,						// Codes

			KEY_SPACE, KEY_APOSTROPHE, KEY_COMMA, KEY_MINUS,			// Special printable characters
			KEY_DOT, KEY_SLASH,												// Key Codes
			KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6,	// Number
			KEY_7, KEY_8, KEY_9,											// Key Codes

			KEY_ESCAPE, KEY_ENTER, KEY_TAB, BACKSPACE, KEY_INSERT,	// Other Characters
			KEY_DELETE, KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP,
			KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END,
			KEY_CAPS_LOCK, KEY_SCROLL_LOCK, KEY_NUM_LOCK,
			KEY_PRINT_SCREEN, KEY_PAUSE, KEY_F1, KEY_F2, KEY_F3,
			KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,
			KEY_F10, KEY_F11, KEY_F12, KEY_KEYPAD_0, KEY_KEYPAD_1,
			KEY_KEYPAD_2, KEY_KEYPAD_3, KEY_KEYPAD_4, KEY_KEYPAD_5,
			KEY_KEYPAD_6, KEY_KEYPAD_7, KEY_KEYPAD_8, KEY_KEYPAD_9,
			KEY_KEYPAD_DP, KEY_KEYPAD_DIVIDE, KEY_KEYPAD_MULTIPLY,
			KEY_KEYPAD_SUBTRACT, KEY_KEYPAD_ADD, KEY_KEYPAD_ENTER,
			KEY_LEFT_SHIFT, KEY_LEFT_CONTROL, KEY_LEFT_ALT,
			KEY_LEFT_SUPER, KEY_RIGHT_SHIFT, KEY_RIGHT_CONTROL,
			KEY_RIGHT_ALT, KEY_RIGHT_SUPER
		};

		bool isKeyPressed(KeyCode key) const;
		bool isKeyTapped(KeyCode key) const;
		bool isMouseButtonPressed(KeyCode button) const;
		bool isMouseButtonTapped(KeyCode button) const;

		// Returns cursor position relative to window, starting from top left corner of the window
		glm::vec2 getCursorPosition() const;
		glm::vec2 getCursorOffset() const;

	protected:
		double cursorX{ 512.f }, cursorY{ 384.f };
		double lastX{ 0.f }, lastY{ 0.f };
		static const std::size_t KEYS_COUNT = 100U;
		std::bitset<KEYS_COUNT> oldKeyStates;
		std::bitset<KEYS_COUNT> newKeyStates;

		virtual glm::vec2 apiGetCursorPosition() const = 0;
		virtual bool apiIsMouseButtonPressed(KeyCode) const = 0;
		virtual bool apiIsKeyPressed(KeyCode) const = 0;

	private:
		static IIOSystem* instance;
	};
}