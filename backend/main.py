import uvicorn
from fastapi import FastAPI, Depends
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import paho.mqtt.client as mqtt
import json
import threading
from sqlalchemy import create_engine, Column, Integer, String, Float, DateTime, func
from sqlalchemy.orm import sessionmaker, Session
from sqlalchemy.ext.declarative import declarative_base
from datetime import datetime

DATABASE_URL = "sqlite:///./database.db"
BROKER_MQTT = "broker.hivemq.com"
BROKER_PORT = 1883
TOPICO_MQTT = "fma/patio/motos"

Base = declarative_base()

class Telemetry(Base):
    __tablename__ = "telemetry"
    id = Column(Integer, primary_key=True, index=True)
    motorcycle_id = Column(String, index=True)
    lat = Column(Float)
    lng = Column(Float)
    status = Column(String)
    timestamp = Column(DateTime, default=datetime.utcnow)

engine = create_engine(DATABASE_URL)
Base.metadata.create_all(bind=engine)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

class TelemetryData(BaseModel):
    motorcycle_id: str
    lat: float
    lng: float
    status: str
    timestamp: datetime

    class Config:
        from_attributes = True

def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("Conectado ao Broker MQTT!")
        client.subscribe(TOPICO_MQTT)
    else:
        print(f"Falha ao conectar, código de retorno {rc}\n")

def on_message(client, userdata, msg):
    print(f"Mensagem recebida no tópico {msg.topic}: {msg.payload.decode()}")
    try:
        payload = json.loads(msg.payload.decode())
        db = SessionLocal()
        db_telemetry = Telemetry(
            motorcycle_id=payload['motorcycle_id'],
            lat=payload['lat'],
            lng=payload['lng'],
            status=payload['status']
        )
        db.add(db_telemetry)
        db.commit()
        db.close()
        print("Dados salvos no banco de dados.")
    except Exception as e:
        print(f"Erro ao processar mensagem ou salvar no banco: {e}")

def run_mqtt_client():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER_MQTT, BROKER_PORT, 60)
    client.loop_forever()

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.on_event("startup")
def startup_event():
    mqtt_thread = threading.Thread(target=run_mqtt_client)
    mqtt_thread.daemon = True
    mqtt_thread.start()

@app.get("/api/telemetry", response_model=list[TelemetryData])
def get_latest_telemetry(db: Session = Depends(get_db)):
    subquery = db.query(
        Telemetry.motorcycle_id,
        func.max(Telemetry.id).label('max_id')
    ).group_by(Telemetry.motorcycle_id).subquery('latest_ids')

    latest_telemetry = db.query(Telemetry).join(
        subquery,
        Telemetry.id == subquery.c.max_id
    ).order_by(Telemetry.timestamp.desc()).all()
    
    return latest_telemetry

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)