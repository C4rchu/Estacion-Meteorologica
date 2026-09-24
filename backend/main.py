from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from .simulator import WeatherSimulator

app = FastAPI()
simulator = WeatherSimulator()

# Servir archivos estáticos del frontend
app.mount("/static", StaticFiles(directory="frontend/static"), name="static")

@app.get("/")
async def read_index():
    return FileResponse("frontend/index.html")

@app.get("/api/weather")
async def get_weather():
    return simulator.get_data()
