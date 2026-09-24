import random
import datetime
import math

class WeatherSimulator:
    def __init__(self):
        # Valores iniciales
        self.state = {
            "temperature": 24.0,
            "humidity": 60.0,
            "pressure": 1013.0,
            "uv": 0.0,
            "wind_speed": 5.0,
            "wind_direction": "N",
            "rain": False
        }
        self.hour = 8 # Simulación de hora para UV

    def _random_walk(self, value, min_val, max_val, step):
        delta = random.uniform(-step, step)
        new_value = value + delta
        return max(min_val, min(new_value, max_val))

    def get_data(self):
        # Evolución gradual
        self.state["temperature"] = self._random_walk(self.state["temperature"], 10, 40, 0.2)
        self.state["humidity"] = self._random_walk(self.state["humidity"], 30, 90, 0.5)
        self.state["pressure"] = self._random_walk(self.state["pressure"], 1000, 1020, 0.3)
        self.state["wind_speed"] = max(0, self._random_walk(self.state["wind_speed"], 0, 50, 2))
        
        # Dirección viento simple
        if random.random() < 0.1:
            directions = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"]
            self.state["wind_direction"] = random.choice(directions)
            
        # UV basado en hora simulada
        self.hour = (self.hour + 0.01) % 24
        if 6 < self.hour < 18:
            # Función seno para simular pico al mediodía
            self.state["uv"] = round(max(0, math.sin((self.hour - 6) / 12 * math.pi) * 10), 1)
        else:
            self.state["uv"] = 0.0
            
        # Lluvia ocasional
        if random.random() < 0.05:
            self.state["rain"] = not self.state["rain"]

        self.state["timestamp"] = datetime.datetime.now().strftime("%H:%M:%S")
        return self.state
