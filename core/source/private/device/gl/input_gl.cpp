#include "precomp.h"

using namespace glm;

InputManager input;

constexpr uint32 num_keys{ 512 };
bool keys_down[num_keys];
bool prev_keys_down[num_keys];
KeyAction keys_action[num_keys];

constexpr uint32 num_mousebuttons{ 8 };
bool mousebuttons_down[num_mousebuttons];
bool prev_mousebuttons_down[num_mousebuttons];
KeyAction mousebuttons_action[num_mousebuttons];

constexpr uint32 max_num_gamepads{ 4 };
bool gamepad_connected[max_num_gamepads];
GLFWgamepadstate gamepad_state[max_num_gamepads];
GLFWgamepadstate prev_gamepad_state[max_num_gamepads];

vec2 mousepos;
float mousewheel = 0;

void cursor_position_callback(GLFWwindow*, double xpos, double ypos)
{
    mousepos.x = (float)xpos;
    mousepos.y = (float)ypos;
}

void scroll_callback(GLFWwindow*, double, double yoffset) { mousewheel += (float)yoffset; }

void key_callback(GLFWwindow*, int key, int, int action, int)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE) keys_action[key] = static_cast<KeyAction>(action);
}

void mousebutton_callback(GLFWwindow*, int button, int action, int)
{
    if (action == GLFW_PRESS || action == GLFW_RELEASE) mousebuttons_action[button] = static_cast<KeyAction>(action);
}

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
}

void InputManager::init(void* _window)
{
    // >:(
    GLFWwindow* window = reinterpret_cast<GLFWwindow*>(_window);

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mousebutton_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void InputManager::update()
{
    // update keyboard key states
    for (int i = 0; i < num_keys; ++i)
    {
        prev_keys_down[i] = keys_down[i];

        if (keys_action[i] == KeyAction::Press)
            keys_down[i] = true;
        else if (keys_action[i] == KeyAction::Release)
            keys_down[i] = false;

        keys_action[i] = KeyAction::None;
    }

    // update mouse button states
    for (int i = 0; i < num_mousebuttons; ++i)
    {
        prev_mousebuttons_down[i] = mousebuttons_down[i];

        if (mousebuttons_action[i] == KeyAction::Press)
            mousebuttons_down[i] = true;
        else if (mousebuttons_action[i] == KeyAction::Release)
            mousebuttons_down[i] = false;

        mousebuttons_action[i] = KeyAction::None;
    }

    // update gamepad states
    for (int i = 0; i < max_num_gamepads; ++i)
    {
        prev_gamepad_state[i] = gamepad_state[i];

        if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
            gamepad_connected[i] = static_cast<bool>(glfwGetGamepadState(i, &gamepad_state[i]));
    }
}

bool InputManager::is_gamepad_available(int gamepadID) const
{
    return gamepad_connected[gamepadID];
}

float InputManager::get_gamepad_axis(int gamepadID, GamepadAxis axis) const
{
    if (!is_gamepad_available(gamepadID)) return 0.0;

    int a = static_cast<int>(axis);
    assert(a >= 0 && a <= GLFW_GAMEPAD_AXIS_LAST);
    return gamepad_state[gamepadID].axes[a];
}

float InputManager::get_gamepad_axis_previous(int gamepadID, GamepadAxis axis) const
{
    if (!is_gamepad_available(gamepadID)) return 0.0;

    int a = static_cast<int>(axis);
    assert(a >= 0 && a <= GLFW_GAMEPAD_AXIS_LAST);
    return prev_gamepad_state[gamepadID].axes[a];
}

bool InputManager::get_gamepad_button(int gamepadID, GamepadButton button) const
{
    if (!is_gamepad_available(gamepadID)) return false;

    int b = static_cast<int>(button);
    assert(b >= 0 && b <= GLFW_GAMEPAD_BUTTON_LAST);
    return static_cast<bool>(gamepad_state[gamepadID].buttons[b]);
}

bool InputManager::get_gamepad_button_once(int gamepadID, GamepadButton button) const
{
    if (!is_gamepad_available(gamepadID)) return false;

    int b = static_cast<int>(button);

    assert(b >= 0 && b <= GLFW_GAMEPAD_BUTTON_LAST);
    return !static_cast<bool>(prev_gamepad_state[gamepadID].buttons[b]) &&
        static_cast<bool>(gamepad_state[gamepadID].buttons[b]);
}

bool InputManager::is_mouse_available() const
{
    return true;
}

bool InputManager::get_mouse_button(MouseButton button) const
{
    int b = static_cast<int>(button);
    return mousebuttons_down[b];
}

bool InputManager::get_mouse_button_once(MouseButton button) const
{
    int b = static_cast<int>(button);
    return mousebuttons_down[b] && !prev_mousebuttons_down[b];
}

vec2 InputManager::get_mouse_position() const
{
    return mousepos;
}

float InputManager::get_mouse_wheel() const
{
    return mousewheel;
}

bool InputManager::is_keyboard_available() const
{
    return true;
}

bool InputManager::get_keyboard_key(KeyboardKey key) const
{
    int k = static_cast<int>(key);
    assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
    return keys_down[k];
}

bool InputManager::get_keyboard_key_once(KeyboardKey key) const
{
    int k = static_cast<int>(key);
    assert(k >= GLFW_KEY_SPACE && k <= GLFW_KEY_LAST);
    return keys_down[k] && !prev_keys_down[k];
}
