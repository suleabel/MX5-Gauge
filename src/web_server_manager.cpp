#include "web_server_manager.h"

#include <LittleFS.h>
#include <WiFi.h>

namespace {
void appendJsonString(String& out, const String& value) {
    out += '"';
    for (size_t i = 0; i < value.length(); ++i) {
        const char c = value[i];
        if (c == '"' || c == '\\') { out += '\\'; out += c; }
        else if (c == '\n') out += F("\\n");
        else if (static_cast<uint8_t>(c) >= 0x20) out += c;
    }
    out += '"';
}
const char* sourceName(SensorSource v) { return v == SensorSource::Analog ? "analog" : "demo"; }
const char* waveName(DemoWaveform v) { switch (v) { case DemoWaveform::Triangle:return "triangle"; case DemoWaveform::Random:return "random"; case DemoWaveform::Constant:return "constant"; default:return "sine"; } }
const char* profileName(ProfileType v) { return v == ProfileType::Multipoint ? "multipoint" : v == ProfileType::Ntc ? "ntc" : "linear"; }
const char* gaugeTypeName(GaugeType v) { switch(v){case GaugeType::AnalogNeedle:return "analog";case GaugeType::DualDigital:return "dual";case GaugeType::MinimalDigital:return "minimal";case GaugeType::DualArc:return "dualArc";case GaugeType::QuadDigital:return "quad";case GaugeType::DualAnalog:return "dualAnalog";default:return "arc";} }
bool boolArg(WebServer& s, const char* n) { return s.hasArg(n) && s.arg(n) != "0" && s.arg(n) != "false"; }
bool validColor(const String& value) { if(value.length()!=7||value[0]!='#')return false;for(uint8_t i=1;i<7;++i)if(!isxdigit(static_cast<unsigned char>(value[i])))return false;return true; }
}

WebServerManager::WebServerManager() : server_(80) {}

void WebServerManager::begin(const AppWiFiManager& wifi, SensorManager& sensors, GaugeManager& gauges) {
    wifiManager_ = &wifi; sensorManager_ = &sensors; gaugeManager_ = &gauges;
    server_.on("/", HTTP_GET, [this]() { File f=LittleFS.open("/index.html","r"); if(!f){server_.send(500,"application/json","{\"error\":\"index_missing\"}");return;} server_.streamFile(f,"text/html; charset=utf-8"); f.close(); });
    server_.on("/api/status", HTTP_GET, [this]() { sendStatus(); });
    server_.on("/api/health", HTTP_GET, [this]() { server_.send(200,"application/json","{\"status\":\"ok\"}"); });
    server_.on("/api/sensors", HTTP_GET, [this]() { sendSensors(); });
    server_.on("/api/sensors", HTTP_POST, [this]() { saveSensor(); });
    server_.on("/api/sensors", HTTP_DELETE, [this]() { bool ok=sensorManager_->removeSensor(server_.arg("id").toInt()); server_.send(ok?200:404,"application/json",ok?"{\"ok\":true}":"{\"error\":\"not_found\"}"); });
    server_.on("/api/profiles", HTTP_GET, [this]() { sendProfiles(); });
    server_.on("/api/profiles", HTTP_POST, [this]() { saveProfile(); });
    server_.on("/api/profiles", HTTP_DELETE, [this]() { bool ok=sensorManager_->removeProfile(server_.arg("id").toInt()); server_.send(ok?200:404,"application/json",ok?"{\"ok\":true}":"{\"error\":\"not_found\"}"); });
    server_.on("/api/readings", HTTP_GET, [this]() { sendReadings(); });
    server_.on("/api/gauges", HTTP_GET, [this]() { sendGauges(); });
    server_.on("/api/gauges", HTTP_POST, [this]() { saveGauge(); });
    server_.on("/api/gauges", HTTP_DELETE, [this]() { bool ok=gaugeManager_->remove(server_.arg("id").toInt());server_.send(ok?200:404,"application/json",ok?"{\"ok\":true}":"{\"error\":\"not_found\"}"); });
    server_.on("/api/gauges/active", HTTP_POST, [this]() { bool ok=gaugeManager_->setActive(server_.arg("id").toInt());server_.send(ok?200:404,"application/json",ok?"{\"ok\":true}":"{\"error\":\"not_found\"}"); });
    server_.on("/api/gauges/move", HTTP_POST, [this]() { bool ok=gaugeManager_->move(server_.arg("id").toInt(),server_.arg("direction")=="up"?-1:1);server_.send(ok?200:400,"application/json",ok?"{\"ok\":true}":"{\"error\":\"cannot_move\"}"); });
    server_.serveStatic("/",LittleFS,"/","max-age=300");
    server_.onNotFound([this]() { sendNotFound(); }); server_.begin();
    Serial.println("HTTP webszerver es Sensor API elindult.");
}

void WebServerManager::handleClient() { server_.handleClient(); }

void WebServerManager::sendStatus() {
    String r; r.reserve(256); r+=F("{\"device\":\"MX-5 Smart Gauge\",\"firmware\":\"0.3.0\",\"display\":false,\"ssid\":"); appendJsonString(r,wifiManager_->ssid());
    r+=F(",\"ip\":"); appendJsonString(r,wifiManager_->ipAddress().toString()); r+=F(",\"clients\":");r+=wifiManager_->connectedClients();r+=F(",\"uptimeSeconds\":");r+=millis()/1000;r+=F(",\"freeHeapBytes\":");r+=ESP.getFreeHeap();r+='}';
    server_.sendHeader("Cache-Control","no-store");server_.send(200,"application/json",r);
}

void WebServerManager::sendSensors() {
    String out("[");out.reserve(2500);
    for(uint8_t i=0;i<sensorManager_->sensorCount();++i){const auto&s=sensorManager_->sensorAt(i);if(i)out+=',';out+=F("{\"id\":");out+=s.id;out+=F(",\"name\":");appendJsonString(out,s.name);out+=F(",\"unit\":");appendJsonString(out,s.unit);out+=F(",\"source\":\"");out+=sourceName(s.source);out+=F("\",\"enabled\":");out+=s.enabled?"true":"false";out+=F(",\"min\":");out+=s.minimum;out+=F(",\"max\":");out+=s.maximum;out+=F(",\"warningLow\":");out+=s.warningLow;out+=F(",\"warningHigh\":");out+=s.warningHigh;out+=F(",\"profileId\":");out+=s.profileId;out+=F(",\"pin\":");out+=s.analogPin;out+=F(",\"waveform\":\"");out+=waveName(s.waveform);out+=F("\",\"speed\":");out+=s.speed;out+=F(",\"constant\":");out+=s.constantValue;out+='}';}out+=']';
    server_.sendHeader("Cache-Control","no-store");server_.send(200,"application/json",out);
}

void WebServerManager::saveSensor() {
    SensorConfig s;s.id=server_.arg("id").toInt();s.name=server_.arg("name");s.unit=server_.arg("unit");s.source=server_.arg("source")=="analog"?SensorSource::Analog:SensorSource::Demo;s.enabled=boolArg(server_,"enabled");s.minimum=server_.arg("min").toFloat();s.maximum=server_.arg("max").toFloat();s.warningLow=server_.arg("warningLow").toFloat();s.warningHigh=server_.arg("warningHigh").toFloat();s.profileId=server_.arg("profileId").toInt();s.analogPin=server_.arg("pin").toInt();
    String w=server_.arg("waveform");s.waveform=w=="triangle"?DemoWaveform::Triangle:w=="random"?DemoWaveform::Random:w=="constant"?DemoWaveform::Constant:DemoWaveform::Sine;s.speed=server_.arg("speed").toFloat();s.constantValue=server_.arg("constant").toFloat();
    if(s.source==SensorSource::Demo)s.profileId=0;
    if(s.name.isEmpty()||s.name.length()>40||s.unit.length()>12||s.minimum>=s.maximum||s.speed<0||(s.source==SensorSource::Analog&&(s.analogPin<32||s.analogPin>39))){server_.send(400,"application/json","{\"error\":\"invalid_sensor\"}");return;}auto*saved=sensorManager_->upsertSensor(s);if(!saved){server_.send(507,"application/json","{\"error\":\"sensor_limit\"}");return;}server_.send(200,"application/json",String("{\"ok\":true,\"id\":")+saved->id+'}');
}

void WebServerManager::sendProfiles() {
    String out("[");out.reserve(2500);for(uint8_t i=0;i<sensorManager_->profileCount();++i){const auto&p=sensorManager_->profileAt(i);if(i)out+=',';out+=F("{\"id\":");out+=p.id;out+=F(",\"name\":");appendJsonString(out,p.name);out+=F(",\"type\":\"");out+=profileName(p.type);out+=F("\",\"points\":[");for(uint8_t j=0;j<p.pointCount;++j){if(j)out+=',';out+='[';out+=p.points[j].input;out+=',';out+=p.points[j].output;out+=']';}out+=F("],\"ntcA\":");out+=String(p.ntcA,9);out+=F(",\"ntcB\":");out+=String(p.ntcB,9);out+=F(",\"ntcC\":");out+=String(p.ntcC,12);out+=F(",\"pullup\":");out+=p.pullupOhms;out+='}';}out+=']';server_.sendHeader("Cache-Control","no-store");server_.send(200,"application/json",out);
}

void WebServerManager::saveProfile() {
    SensorProfile p;p.id=server_.arg("id").toInt();p.name=server_.arg("name");String t=server_.arg("type");p.type=t=="multipoint"?ProfileType::Multipoint:t=="ntc"?ProfileType::Ntc:ProfileType::Linear;p.ntcA=server_.arg("ntcA").toFloat();p.ntcB=server_.arg("ntcB").toFloat();p.ntcC=server_.arg("ntcC").toFloat();p.pullupOhms=server_.arg("pullup").toFloat();
    String pts=server_.arg("points");int start=0;while(start<(int)pts.length()&&p.pointCount<12){int end=pts.indexOf(';',start);if(end<0)end=pts.length();String pair=pts.substring(start,end);int colon=pair.indexOf(':');if(colon>0)p.points[p.pointCount++]={pair.substring(0,colon).toFloat(),pair.substring(colon+1).toFloat()};start=end+1;}
    if(p.name.isEmpty()||(p.type!=ProfileType::Ntc&&p.pointCount<2)||(p.type==ProfileType::Ntc&&(p.ntcA<=0||p.ntcB<=0||p.pullupOhms<=0))){server_.send(400,"application/json","{\"error\":\"invalid_profile\"}");return;}for(uint8_t i=1;i<p.pointCount;++i)if(p.points[i].input<=p.points[i-1].input){server_.send(400,"application/json","{\"error\":\"points_not_sorted\"}");return;}auto*saved=sensorManager_->upsertProfile(p);if(!saved){server_.send(507,"application/json","{\"error\":\"profile_limit\"}");return;}server_.send(200,"application/json",String("{\"ok\":true,\"id\":")+saved->id+'}');
}

void WebServerManager::sendReadings() {
    String out("{\"timestamp\":");out+=millis();out+=F(",\"values\":[");for(uint8_t i=0;i<sensorManager_->sensorCount();++i){const auto&s=sensorManager_->sensorAt(i);if(i)out+=',';out+=F("{\"id\":");out+=s.id;out+=F(",\"value\":");if(isnan(s.currentValue))out+="null";else out+=String(s.currentValue,2);out+=F(",\"enabled\":");out+=s.enabled?"true":"false";out+='}';}out+=F("]}");server_.sendHeader("Cache-Control","no-store");server_.send(200,"application/json",out);
}

void WebServerManager::sendGauges(){
    String out("{\"activeId\":");out+=gaugeManager_->activeId();out+=F(",\"gauges\":[");out.reserve(9000);
    for(uint8_t i=0;i<gaugeManager_->count();++i){const auto&g=gaugeManager_->gaugeAt(i);if(i)out+=',';out+=F("{\"id\":");out+=g.id;out+=F(",\"name\":");appendJsonString(out,g.name);out+=F(",\"type\":\"");out+=gaugeTypeName(g.type);out+=F("\",\"backgroundColor\":");appendJsonString(out,g.backgroundColor);out+=F(",\"accentColor\":");appendJsonString(out,g.accentColor);out+=F(",\"textColor\":");appendJsonString(out,g.textColor);out+=F(",\"warningColor\":");appendJsonString(out,g.warningColor);out+=F(",\"showUnit\":");out+=g.showUnit?"true":"false";out+=F(",\"slots\":[");
        for(uint8_t j=0;j<4;++j){if(j)out+=',';const auto&s=g.slots[j];out+=F("{\"sensorId\":");out+=s.sensorId;out+=F(",\"min\":");out+=s.minimum;out+=F(",\"max\":");out+=s.maximum;out+=F(",\"startAngle\":");out+=s.startAngle;out+=F(",\"clockwise\":");out+=s.clockwise?"true":"false";out+=F(",\"nameScale\":");out+=s.nameScale;out+=F(",\"nameOffsetX\":");out+=s.nameOffsetX;out+=F(",\"nameOffsetY\":");out+=s.nameOffsetY;out+=F(",\"valueScale\":");out+=s.valueScale;out+=F(",\"valueOffsetX\":");out+=s.valueOffsetX;out+=F(",\"valueOffsetY\":");out+=s.valueOffsetY;out+=F(",\"unitScale\":");out+=s.unitScale;out+=F(",\"unitOffsetX\":");out+=s.unitOffsetX;out+=F(",\"unitOffsetY\":");out+=s.unitOffsetY;out+='}';}out+=F("]}");}
    out+=F("]}");server_.sendHeader("Cache-Control","no-store");server_.send(200,"application/json",out);
}

void WebServerManager::saveGauge(){
    GaugeConfig g;g.id=server_.arg("id").toInt();g.name=server_.arg("name");String type=server_.arg("type");g.type=type=="analog"?GaugeType::AnalogNeedle:type=="dual"?GaugeType::DualDigital:type=="minimal"?GaugeType::MinimalDigital:type=="dualArc"?GaugeType::DualArc:type=="quad"?GaugeType::QuadDigital:type=="dualAnalog"?GaugeType::DualAnalog:GaugeType::ArcDigital;g.primarySensorId=server_.arg("primarySensorId").toInt();g.secondarySensorId=server_.arg("secondarySensorId").toInt();g.tertiarySensorId=server_.arg("tertiarySensorId").toInt();g.quaternarySensorId=server_.arg("quaternarySensorId").toInt();g.backgroundColor=server_.arg("backgroundColor");g.accentColor=server_.arg("accentColor");g.textColor=server_.arg("textColor");g.warningColor=server_.arg("warningColor");g.scaleMinimum=server_.arg("scaleMin").toFloat();g.scaleMaximum=server_.arg("scaleMax").toFloat();g.showUnit=boolArg(server_,"showUnit");g.nameScale=server_.arg("nameScale").toInt();g.nameOffsetX=server_.arg("nameOffsetX").toInt();g.nameOffsetY=server_.arg("nameOffsetY").toInt();g.valueScale=server_.arg("valueScale").toInt();g.valueOffsetX=server_.arg("valueOffsetX").toInt();g.valueOffsetY=server_.arg("valueOffsetY").toInt();g.unitScale=server_.arg("unitScale").toInt();g.unitOffsetX=server_.arg("unitOffsetX").toInt();g.unitOffsetY=server_.arg("unitOffsetY").toInt();
    for(uint8_t i=0;i<4;++i){String p="s"+String(i);auto&s=g.slots[i];s.sensorId=server_.arg(p+"SensorId").toInt();s.minimum=server_.arg(p+"Min").toFloat();s.maximum=server_.arg(p+"Max").toFloat();s.startAngle=server_.arg(p+"Start").toInt();s.clockwise=server_.arg(p+"Direction")!="ccw";s.nameScale=server_.arg(p+"NameScale").toInt();s.nameOffsetX=server_.arg(p+"NameX").toInt();s.nameOffsetY=server_.arg(p+"NameY").toInt();s.valueScale=server_.arg(p+"ValueScale").toInt();s.valueOffsetX=server_.arg(p+"ValueX").toInt();s.valueOffsetY=server_.arg(p+"ValueY").toInt();s.unitScale=server_.arg(p+"UnitScale").toInt();s.unitOffsetX=server_.arg(p+"UnitX").toInt();s.unitOffsetY=server_.arg(p+"UnitY").toInt();}
    g.primarySensorId=g.slots[0].sensorId;g.secondarySensorId=g.slots[1].sensorId;g.tertiarySensorId=g.slots[2].sensorId;g.quaternarySensorId=g.slots[3].sensorId;g.scaleMinimum=g.slots[0].minimum;g.scaleMaximum=g.slots[0].maximum;g.nameScale=g.slots[0].nameScale;g.nameOffsetX=g.slots[0].nameOffsetX;g.nameOffsetY=g.slots[0].nameOffsetY;g.valueScale=g.slots[0].valueScale;g.valueOffsetX=g.slots[0].valueOffsetX;g.valueOffsetY=g.slots[0].valueOffsetY;g.unitScale=g.slots[0].unitScale;g.unitOffsetX=g.slots[0].unitOffsetX;g.unitOffsetY=g.slots[0].unitOffsetY;
    uint8_t usedSlots=g.type==GaugeType::QuadDigital?4:(g.type==GaugeType::DualDigital||g.type==GaugeType::DualArc||g.type==GaugeType::DualAnalog?2:1);
    for(uint8_t i=0;i<usedSlots;++i){const auto&s=g.slots[i];bool angleOk=s.startAngle==-90||s.startAngle==0||s.startAngle==90||s.startAngle==180;if(!sensorManager_->sensor(s.sensorId)||s.minimum>=s.maximum||!angleOk||s.nameScale<50||s.nameScale>200||s.valueScale<50||s.valueScale>200||s.unitScale<50||s.unitScale>200||abs(s.nameOffsetX)>100||abs(s.nameOffsetY)>100||abs(s.valueOffsetX)>100||abs(s.valueOffsetY)>100||abs(s.unitOffsetX)>100||abs(s.unitOffsetY)>100){server_.send(400,"application/json","{\"error\":\"invalid_sensor_slot\"}");return;}}
    if(g.name.isEmpty()||g.name.length()>40||g.scaleMinimum>=g.scaleMaximum||g.nameScale<50||g.nameScale>200||g.valueScale<50||g.valueScale>200||g.unitScale<50||g.unitScale>200||abs(g.nameOffsetX)>100||abs(g.nameOffsetY)>100||abs(g.valueOffsetX)>100||abs(g.valueOffsetY)>100||abs(g.unitOffsetX)>100||abs(g.unitOffsetY)>100||!sensorManager_->sensor(g.primarySensorId)||!validColor(g.backgroundColor)||!validColor(g.accentColor)||!validColor(g.textColor)||!validColor(g.warningColor)){server_.send(400,"application/json","{\"error\":\"invalid_gauge\"}");return;}if((g.type==GaugeType::DualDigital||g.type==GaugeType::DualArc||g.type==GaugeType::DualAnalog||g.type==GaugeType::QuadDigital)&&!sensorManager_->sensor(g.secondarySensorId)){server_.send(400,"application/json","{\"error\":\"secondary_sensor_required\"}");return;}if(g.type==GaugeType::QuadDigital&&(!sensorManager_->sensor(g.tertiarySensorId)||!sensorManager_->sensor(g.quaternarySensorId))){server_.send(400,"application/json","{\"error\":\"four_sensors_required\"}");return;}auto*saved=gaugeManager_->upsert(g);if(!saved){server_.send(507,"application/json","{\"error\":\"gauge_limit\"}");return;}server_.send(200,"application/json",String("{\"ok\":true,\"id\":")+saved->id+'}');
}
void WebServerManager::sendNotFound(){server_.send(404,"application/json","{\"error\":\"not_found\"}");}
