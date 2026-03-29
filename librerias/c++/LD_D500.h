/**
 * @file LD_D500.h
 * @version 0.1
 * @date 2026-03-28
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef LD_D500_H
#define LD_D500_H
#include <Arduino.h>
#include <vector>
#include <math.h>
#include "HardwareSerial.h"

using namespace std;

// Cabecera del paquete.
#define SOF 0x54
// Número de puntos que se devuelve en cada paquete por UART.
#define NUM_PUNTOS 12


typedef struct
{
    // Vector con los ángulos donde se ha medido.
    vector<float> angulos;
    // Vector con las distancias en mm que se han leído.
    vector<int> distancias;
    // Vector con las intensidades que se han leído.
    vector<int> intensidades;
} LD_D500_return;

class LD_D500
{
private:
    HardwareSerial uartLD_D500;

    // Valor de intensidad mínima para considerar el valor leído.
    int _min_intensidad = 0;
    // Valor de distancia máximo considerado a nivel de lectura de muestras.
    int _max_distancia = 20000;
    // Tamaño del buffer de lectura.
    int _max_size = 0;

public:
    /**
     * @brief Constructor de la clase.
     *
     * @code
     *
     * #include "LD_D500.h"
     *
     * #define TX 21
     * #define RX 22
     * #define SIZE 2048
     *
     * LD_D500 lidar(TX, RX, SIZE);
     *
     * @endcode
     * 
     * @param TX Pin de transmisión.
     * @param RX Pin de recepción.
     * @param size Tamaño del buffer de lectura.
     */
    LD_D500(int TX, int RX, int size);


    /**
     * @brief Constructor de la clase.
     *
     * @code
     *
     * #include "LD_D500.h"
     *
     * #define TX 21
     * #define RX 22
     * #define SIZE 2048
     * #define MIN_INTENSIDAD 10
     *
     * LD_D500 lidar(TX, RX, SIZE, MIN_INTENSIDAD);
     *
     * @endcode
     * 
     * @param TX Pin de transmisión.
     * @param RX Pin de recepción.
     * @param size Tamaño del buffer de lectura.
     */
    LD_D500(int TX, int RX, int size, int min_intensidad);

    /**
     * @brief Destructor de la clase.
     *
     */
    ~LD_D500();

    /**
     * @brief Este método lee los valores que vienen del lidar.
     * 
     * @note El lidar devuelve la siguiente cadena:
     * 
     * | Posición | Valor |
     * |----------|-------|
     * |    0	  | 0x54 |
     * |    1	  | Tipo de mensaje (0x2C) |
     * |    2	  | Velocidad (LSB) |
     * |    3	  | Velocidad |
     * |    4	  | Ángulo de inicio x100 (LSB) |
     * |    5	  | Ángulo de inicio x100 (MSB) |
     * |    6	  | Distancia 1 [mm] (LSB) |
     * |    7	  | Distancia 1 [mm] (MSB) |
     * |    8	  | Intensidad 1 |
     * |    9	  | Distancia 2 [mm] (LSB) |
     * |    10    | Distancia 2 [mm] (MSB) |
     * |    11    | Intensidad 2 |
     * |    12    | Distancia 3 [mm] (LSB) |
     * |    13    | Distancia 3 [mm] (MSB) |
     * |    14    | Intensidad 3 |
     * |    15    | Distancia 4 [mm] (LSB) |
     * |    16    | Distancia 4 [mm] (MSB) |
     * |    17    | Intensidad 4 |
     * |    18    | Distancia 5 [mm] (LSB) |
     * |    19    | Distancia 5 [mm] (MSB) |
     * |    20    | Intensidad 5 |
     * |    21    | Distancia 6 [mm] (LSB) |
     * |    22    | Distancia 6 [mm] (MSB) |
     * |    23    | Intensidad 6 |
     * |    24    | Distancia 7 [mm] (LSB) |
     * |    25    | Distancia 7 [mm] (MSB) |
     * |    26    | Intensidad 7 |
     * |    27    | Distancia 8 [mm] (LSB) |
     * |    28    | Distancia 8 [mm] (MSB) |
     * |    29    | Intensidad 8 |
     * |    30    | Distancia 9 [mm] (LSB) |
     * |    31    | Distancia 9 [mm] (MSB) |
     * |    32    | Intensidad 9 |
     * |    33    | Distancia 10 [mm] (LSB) |
     * |    34    | Distancia 10 [mm] (MSB) |
     * |    35    | Intensidad 10 |
     * |    36    | Distancia 11 [mm] (LSB) |
     * |    37    | Distancia 11 [mm] (MSB) |
     * |    38    | Intensidad 11 |
     * |    39    | Distancia 12 [mm] (LSB) |
     * |    40    | Distancia 12 [mm] (MSB) |
     * |    41    | Intensidad 12 |
     * |    42    | Ángulo de final x100 (LSB) |
     * |    43    | Ángulo de final x100 (MSB) |
     * |    44    | Marca de tiempo (LSB)	 |
     * |    45    | Marca de tiempo (MSB)	 |
     * |    46    | CRC	 |
     * 
     * Example:
     * @code
     *
     * #include "LD_D500.h"
     * #include <vector>
     *
     * #define TX 21
     * #define RX 22
     * #define SIZE 2048
     * 
     * using namespace std;
     * 
     * vector<float> angulos;
     * vector<int> distancias;
     * vector<int> intensidades;
     *
     * LD_D500 lidar(TX, RX, SIZE);
     * 
     * LD_D500_return retorno;
     * 
     *
     * ...
     * retorno = lidar.read_value();
     * 
     * angulos = retorno.angulos;
     * distancias = retorno.distancias;
     * intensidades = retorno.intensidades;
     *
     * @endcode
     *
     * @return LD_D500_return Estructura con los datos de retorno.
     */
    LD_D500_return read_value();
};

#endif