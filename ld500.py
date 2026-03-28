import serial
import matplotlib.pyplot as plt
import numpy as np
import time

class LD500:
    '''
    Esta clase lee, interpreta y pinta los valores recibidos por el lidar LD500.
    '''

# Constantes
    SoF = 0x54
    NUM_PUNTOS = 12

    def __init__(self, port: str="/dev/ttyUSB0", tam_buffer: int= 2048) -> None:
        """
        Este es el constructor de la clase LD500.

        Args:
            port (str): Puerto de la UART para leer. Defaults to "/dev/ttyUSB0".
            buffer_size (int): Tamaño del buffer de lectura por la UART. Defaults to 2048.
        """

        # Apertura del puerto serie del lidar.
        try:
            self.uart = serial.Serial(port=port, baudrate=230400, timeout=1)
        except Exception as e:
            print(f"Error al abrir el puerto: {e}")

        # Definición del tamaño del buffer.
        self.tam_buffer = tam_buffer

        # Valor de intensidad mínima para considerar el valor leído.
        self.min_intensidad = 0
        # Valor de distancia máximo considerado a nivel de lectura de muestras.
        self.max_distancia = 20000


        # Definición de la proyección polar de las muestras.
        self.fig, self.ax = plt.subplots(subplot_kw={'projection': 'polar'})
        # Definición de la ventana a pintar.
        self.scatter = None
        self.max_distancia_pintar = 10000  # Limite del radio a pintar en mm.
        self.tam_puntos = 1  # Tamaño de los puntos a pintar.
        self.angulo_inicio = 359  # Ángulo de inicio en grados.

        # Definición del radio límite y su dirección en el eje Y.
        self.ax.set_ylim(0, self.max_distancia_pintar) 
        self.ax.set_title('Datos en tiempo real')

        # Ajustar el inicio al 0.
        # Poner el 0 en coordenadas polares.
        self.ax.set_theta_offset(np.radians(self.angulo_inicio))
        # Pintar los datos en sentido horario.
        self.ax.set_theta_direction(-1)  




    def read_value(self) -> tuple[list, list, list]:
        """Este método lee los valores que vienen del lidar.

        Returns:
            tuple[distancias, intensidades, angulos]: 
                distancias : cadena con los valores de las distancias.
                intensidades : cadena con los valores de las intensidades.
                angulos : cadena con los valores de los ángulos.

        Example:
            >>> lidar = LD500()

            >>> while(True):
            >>>     distancia, intensidad, angulos = lidar.read_value()
        """
        # Definición de variables iniciales.
        distancias = []
        intensidades = []
        angulos = []
        distancia = 0
        intensidad = 0

        # Lectura de los datos de la UART.
        data = self.uart.read(self.tam_buffer)



        # Si el valor leído es menor de 47 no ejecutamos nada.
        while len(data)>= 47:

            # Se busca el valor comienzo de la cadena.
            indice = data.find(self.SoF)
            # Si no se encuentra salimos.
            if indice == -1:
                break

            # Extraemos la cadena de 47 caracteres.
            cadena = data[indice: indice+47]
            # Regeneramos los datos para la siguiente lectura.
            data = data[indice+47:]

            # Si los datos que quedan en la cadena son menos que una cadena salimos del bucle.
            if len(data)< indice + 47:
                break

            # Lectura del ángulo inicial.
            angulo_inicial = int.from_bytes(cadena[4:6], byteorder='little')/100.0
            # Lectura del ángulo final.
            angulo_final = int.from_bytes(cadena[42:44], byteorder='little')/100.0

            # Si la diferencia de ángulos en negativa retornamos al inicio.
            if angulo_final-angulo_inicial < 0:
                inc_angulo = (abs(((360+angulo_final)-angulo_inicial)) % 360.0)/(self.NUM_PUNTOS-1)
            else:
                inc_angulo = (abs((angulo_final-angulo_inicial)) % 360.0)/(self.NUM_PUNTOS-1)

            # Se lee interpretan los valores ángulo a ángulo.
            for i in range(self.NUM_PUNTOS):
                # Leemos el valor de la posisición teniendo en cuenta el offset de la cadena de entrada.
                posicion = 6 + i*3
                # Obtenemos el ángulo de cada iteración.
                angulo = angulo_inicial + i*inc_angulo

                # Si el ángulo en la iteración es mayor de 360, lo devolvemos al indice.
                if angulo > 360:
                    angulo -= 360

                # Extraemos el valor de la distancia.
                distancia = int.from_bytes(cadena[posicion:posicion+2], byteorder='little')
                # Extraemos el valor de la intensidad.
                intensidad = cadena[posicion+2]

                # Si el valor de la intensidad es menor que el valor mínimo lo ponemos a 0.
                if intensidad < self.min_intensidad:
                    intensidad = 0

                # Si el valor de la distancia es 0, le adjudicamos el valor máximo.
                if distancia == 0:
                    distancia = self.max_distancia

                # Asignamos los valores leídos a las cadenas de retorno.
                distancias.append(distancia)
                intensidades.append(intensidad)
                angulos.append(angulo)

        # Retornamos los valores.
        return distancias, intensidades, angulos




    def plot_data(self, angulos, distancias) -> None:
        """ Este método pinta los valores obtenidos del lidar.
        
        Args:
            angulos (list): Lista con el valor de los ángulos.
            distancias (list): lista con el valor de las distancias.

        Returns:
            None


        Example: 
            >>> lidar = LD500()
            
            >>> while(True):
            >>>     distancia, intensidad, angulos = lidar.read_value()
            >>>     lidar.plot_data(angulos, distancia)

        """

        # Conversión de los ángulos a radianes.
        angulos_validos = [np.radians(angulo) for angulo, distance in zip(angulos, distancias) if distance <= self.max_distancia_pintar]
        # Si las distancias están por debajo de la distancia máxima, se da como válidas las distancias.
        distancias_validas = [distancia for distancia in distancias if distancia <= self.max_distancia_pintar]


        if not angulos_validos or not distancias_validas:
            return 

        # Si es la primera vez pintamos los valores.
        if self.scatter is None:
            self.scatter = self.ax.scatter(angulos_validos, distancias_validas, s=self.tam_puntos, c='b', alpha=0.75)
        else:
            # Si no, actualizamos los valores.
            self.scatter.set_offsets(np.c_[angulos_validos, distancias_validas])

        #  Actualizamos los valores.
        plt.pause(0.01)
        # Esperamos para evitar problemas al pintar.
        time.sleep(0.01)


