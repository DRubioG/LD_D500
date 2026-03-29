/**
 * @file LD_D500.cpp
 * @version 0.1
 * @date 2026-03-28
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "LD_D500.h"

// Constructor de la clase.
LD_D500::LD_D500(int TX, int RX, int size) : uartLD_D500(1)
{
// Configuración de la UART del lidar.
    uartLD_D500.begin(230400, SERIAL_8N1, RX, TX);
// Asignación del tamaño máximo del buffer de lectura de la UART.
    _max_size = size;
}

// Constructor de la clase.
LD_D500::LD_D500(int TX, int RX, int size, int min_intensidad) : uartLD_D500(1)
{
// Configuración de la UART del lidar.
    uartLD_D500.begin(230400, SERIAL_8N1, RX, TX);
// Asignación del tamaño máximo del buffer de lectura de la UART.
    _max_size = size;
// Asignación del mínimo de la intensidad.
    _min_intensidad = min_intensidad;
}

// Destructor de la clase.
LD_D500::~LD_D500()
{
}

/**
 * @brief Esta función devuelve el valor de la posición en la que se encuentra el primer caracter.
 * 
 * @param cadena vector con los datos de entrada.
 * @param caracter Caracter a buscar en la cadena.
 * @return int Valor del índice donde está el caracter. En caso de no encontrarse, se devuelve un -1.
 */
int find(vector<uint8_t> cadena, uint8_t caracter)
{
// Se recorre la cadena caracter a caracter.
    for (int i = 0; i < cadena.size(); i++)
    {
        // Si se encuetra el caracter, se devuelve el índice.
        if (cadena.at(i) == caracter)
        {
            return i;
        }
    }
    // si no se encuentra ninguna posición con el valor del caracter.
    return -1;
}

// Este método lee los valores que vienen del lidar.
LD_D500_return LD_D500::read_value()
{
    // Definicion de variables.
    LD_D500_return retorno;
    uint8_t buffer[_max_size]; // ajusta tamaño
    vector<uint8_t> cadena, data;



    // Lectura de los datos de la UART.
    int bytesLeidos = uartLD_D500.readBytes(buffer, _max_size);
    // Cambiamos el array por un vector.
    data.assign(buffer, buffer + bytesLeidos);

    // Si el valor leído es menor de 47 no ejecutamos nada.
    while (data.size() >= 47)
    {

        // Se busca el valor comienzo de la cadena.
        int indice = find(data, SOF);
        // Si no se encuentra salimos.
        if (indice == -1)
            return retorno;

        // Extraemos la cadena de 47 caracteres.
        cadena.assign(data.begin() + indice, data.begin() + indice + 47);

        // Regeneramos los datos para la siguiente lectura.
        data.erase(data.begin(), data.begin() + indice + 47);

        // Si los datos que quedan en la cadena son menos que una cadena salimos del bucle.
        if (data.size() < indice+47)
        {
            break;
        }
        


        // Lectura del ángulo inicial.
        float angulo_inicial = (static_cast<uint16_t>(cadena[4]) | (static_cast<uint16_t>(cadena[5]) << 8)) / 100.0;
        // Lectura del ángulo final.
        float angulo_final = (static_cast<uint16_t>(cadena[42]) | (static_cast<uint16_t>(cadena[43]) << 8)) / 100.0;

        float inc_angulo = 0;
        //Si la diferencia de ángulos en negativa retornamos al inicio.
        if (angulo_final - angulo_inicial < 0)
        {
            inc_angulo = fmod(fabs((360 + angulo_final) - angulo_inicial), 360.0) / (NUM_PUNTOS - 1);
        }
        else
        {
            inc_angulo = fmod(fabs(angulo_final - angulo_inicial), 360.0) / (NUM_PUNTOS - 1);
        }
        // Se lee interpretan los valores ángulo a ángulo.
        for (int i = 0; i < NUM_PUNTOS; i++)
        {
            // Leemos el valor de la posisición teniendo en cuenta el offset de la cadena de entrada.
            float posicion = 6 + i * 3;
            // Obtenemos el ángulo de cada iteración.
            float angulo = angulo_inicial + i * inc_angulo;

            // Si el ángulo en la iteración es mayor de 360, lo devolvemos al indice.
            if (angulo > 360)
                angulo -= 360;

            // Extraemos el valor de la distancia en mm.
            int distancia = static_cast<int>(static_cast<uint16_t>(cadena[posicion]) | (static_cast<uint16_t>(cadena[posicion + 1]) << 8));
            // Extraemos el valor de la intensidad.
            int intensidad = cadena.at(posicion + 2);

            // Si el valor de la intensidad es menor que el valor mínimo lo ponemos a 0.
            if (intensidad < _min_intensidad)
                intensidad = 0;

            // Si el valor de la distancia es 0, le adjudicamos el valor máximo.
            if (distancia == 0)
                distancia = _max_distancia;

            // Asignamos los valores leídos a las cadenas de retorno.
            retorno.distancias.push_back(distancia);
            retorno.intensidades.push_back(intensidad);
            retorno.angulos.push_back(angulo);
        }
    }

    return retorno;
}
