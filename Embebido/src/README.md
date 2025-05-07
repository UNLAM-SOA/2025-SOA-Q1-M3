# Directorio con el código fuente del Sistema Embebido

Para el desarrollo y simulación del firmware se utilizó Visual Studio Code con las extensiones PlatformIO y Wokwi. PlatformIO se empleó como entorno de construcción y gestión de dependencias, permitiendo compilar y organizar el código fuente para la plataforma ESP32. La simulación del circuito y el comportamiento del microcontrolador se realizó con Wokwi, que ofrece un entorno virtual interactivo sin necesidad de hardware físico.

El archivo `State_machine_generator.xlsm` contiene una rapresentacion visual de las tablas de acciones y proximos estados, el cual, a su vez, tiene una macro que genera el codigo C necesario para llevar esas tablas al codigo. Esto fue usado durante el desarrollo para tener un mejor seguimiento de los estados y acciones y para que sea mas facil hacer cambios cuendo fuera necesario

> [!TIP]
> En el archivo `event_types.h` hay un define `ENABLE_PERIODICAL_TIME_EVENTS` que sirve para que constantemente este lanzando eventos de tiempo cada cierto intervalo de tiempo (definido en `PERIODICAL_TIME_EVENTS_TIME`). Esto se hizo solo con el fin de facilitar el testeo del sistema y no tener que esperar a que llegue un horario previamente seteado
