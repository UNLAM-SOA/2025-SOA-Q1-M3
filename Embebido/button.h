enum ButtonMode
{
    CLICK_MODE,
    DURATION_MODE
};

enum ButtonState
{
    IDLE,
    PRESSING,
    WAITING_BETWEEN_CLICKS,
    DONE
};

struct ButtonAction
{
    uint8_t pin;
    ButtonMode mode;
    uint16_t value; // cantidad de clicks o duración en ms
    ButtonState state;
    uint32_t lastChange;
    uint16_t clickCount;
    bool active;
};

// Inicializa una acción
void startButtonAction(ButtonAction &action, uint8_t pin, ButtonMode mode, uint16_t value)
{
    action.pin = pin;
    action.mode = mode;
    action.value = value;
    action.state = PRESSING;
    action.lastChange = millis();
    action.clickCount = 0;
    action.active = true;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // inicia el primer "click" o la presión sostenida
}

// Ejecuta la acción de forma no bloqueante
void updateButtonAction(ButtonAction &action)
{
    if (!action.active)
        return;

    uint32_t now = millis();

    switch (action.mode)
    {
    case CLICK_MODE:
        if (action.state == PRESSING && now - action.lastChange >= 100)
        {
            digitalWrite(action.pin, HIGH); // suelta
            action.lastChange = now;
            action.state = WAITING_BETWEEN_CLICKS;
        }
        else if (action.state == WAITING_BETWEEN_CLICKS && now - action.lastChange >= 100)
        {
            action.clickCount++;
            if (action.clickCount >= action.value)
            {
                action.state = DONE;
                action.active = false;
            }
            else
            {
                digitalWrite(action.pin, LOW); // nuevo click
                action.lastChange = now;
                action.state = PRESSING;
            }
        }
        break;

    case DURATION_MODE:
        if (action.state == PRESSING && now - action.lastChange >= action.value)
        {
            digitalWrite(action.pin, HIGH); // suelta
            action.state = DONE;
            action.active = false;
        }
        break;
    }
}
void setup()
{
    // Simula 3 clicks en el pin 15
    startButtonAction(button, 15, CLICK_MODE, 3);

    // O: simula presión sostenida por 1500 ms
    // startButtonAction(button, 15, DURATION_MODE, 1500);
}

void loop()
{
    updateButtonAction(button);

    // Aquí podés seguir ejecutando otras tareas
}