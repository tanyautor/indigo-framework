#pragma once


enum class KeyboardKey
{
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Digit0 = 48,
    Digit1 = 49,
    Digit2 = 50,
    Digit3 = 51,
    Digit4 = 52,
    Digit5 = 53,
    Digit6 = 54,
    Digit7 = 55,
    Digit8 = 56,
    Digit9 = 57,
    Semicolon = 59,
    Equal = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    World1 = 161,
    World2 = 162,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    ArrowRight = 262,
    ArrowLeft = 263,
    ArrowDown = 264,
    ArrowUp = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    Numpad0 = 320,
    Numpad1 = 321,
    Numpad2 = 322,
    Numpad3 = 323,
    Numpad4 = 324,
    Numpad5 = 325,
    Numpad6 = 326,
    Numpad7 = 327,
    Numpad8 = 328,
    Numpad9 = 329,
    NumpadDecimal = 330,
    NumpadDivide = 331,
    NumpadMultiply = 332,
    NumpadSubtract = 333,
    NumpadAdd = 334,
    NumpadEnter = 335,
    NumpadEqual = 336,
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348
};

enum class GamepadAxis
{
    /// Represents the horizontal axis of the left gamepad stick, with an analog input value between -1 (left) and 1
    /// (right).
    StickLeftX = 0,
    /// Represents the vertical axis of the left gamepad stick, with an analog input value between -1 (down) and 1 (up).
    StickLeftY = 1,
    /// Represents the horizontal axis of the right gamepad stick, with an analog input value between -1 (left) and 1
    /// (right).
    StickRightX = 2,
    /// Represents the vertical axis of the right gamepad stick, with an analog input value between -1 (down) and 1 (up).
    StickRightY = 3,
    /// Represents the left trigger of a gamepad, with an analog input value between 0 (not pressed) and 1 (fully pressed).
    TriggerLeft = 4,
    /// Represents the right trigger of a gamepad, with an analog input value between 0 (not pressed) and 1 (fully pressed).
    TriggerRight = 5
};

enum class GamepadButton
{
    /// Represents the bottom (south) button of the 4 main action buttons on a gamepad.
    South = 0,
    /// Represents the right (east) button of the 4 main action buttons on a gamepad.
    East = 1,
    /// Represents the left (west) button of the 4 main action buttons on a gamepad.
    West = 2,
    /// Represents the top (north) button of the 4 main action buttons on a gamepad.
    North = 3,

    /// Represents the left shoulder button on a gamepad.
    ShoulderLeft = 4,
    /// Represents the right shoulder button on a gamepad.
    ShoulderRight = 5,

    /// Represents the left of the two menu-related buttons on a gamepad.
    /// This button has different names on different platforms, such as Share (Xbox) or - (Switch). The PS5 does not have
    /// such a button.
    MenuLeft = 6,
    /// Represents the right of the two menu-related buttons on a gamepad.
    /// This button has different names on different platforms, such as Menu (Xbox), Start (PS5), or + (Switch).
    MenuRight = 7,

    // Button 8 is not used, so that we have a 1-on-1 mapping with the GLFW enum.

    /// Represents the pressing of the left gamepad stick.
    StickPressLeft = 9,
    /// Represents the pressing of the right gamepad stick.
    StickPressRight = 10,

    /// Represents the up arrow of the D-pad on a gamepad.
    DPadUp = 11,
    /// Represents the right arrow of the D-pad on a gamepad.
    DPadRight = 12,
    /// Represents the down arrow of the D-pad on a gamepad.
    DPadDown = 13,
    /// Represents the left arrow of the D-pad on a gamepad.
    DPadLeft = 14
};

enum class MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2
};


enum KeyAction
{
	Release = 0,
	Press = 1,
	None = 2
};


class InputManager
{
public:

    void init(void* window);
	void update();

    // Gamepad
    bool is_gamepad_available(int gamepadID) const;
    float get_gamepad_axis(int gamepadID, GamepadAxis axis) const;
    float get_gamepad_axis_previous(int gamepadID, GamepadAxis axis) const;
    bool get_gamepad_button(int gamepadID, GamepadButton button) const;
    bool get_gamepad_button_once(int gamepadID, GamepadButton button) const;

    // Mouse 
    bool is_mouse_available() const;
    bool get_mouse_button(MouseButton button) const;
    bool get_mouse_button_once(MouseButton button) const;
    glm::vec2 get_mouse_position() const;
    float get_mouse_wheel() const;

    // Keyboard
    bool is_keyboard_available() const;
    bool get_keyboard_key(KeyboardKey button) const;
    bool get_keyboard_key_once(KeyboardKey button) const;

    InputManager();
    ~InputManager();

private:
};

extern InputManager input;