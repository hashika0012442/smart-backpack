import { useEffect, useState } from "react";
import { onValue, ref } from "firebase/database";
import { database } from "./firebase";
import "./App.css";

function App() {
  const [data, setData] = useState({});
  const [time, setTime] = useState(new Date());

  const [temperatureHistory, setTemperatureHistory] = useState([]);
  const [humidityHistory, setHumidityHistory] = useState([]);

  // ==========================================
  // FIREBASE REALTIME DATABASE
  // ==========================================

  useEffect(() => {
    const databaseRef = ref(database, "/");

    const unsubscribe = onValue(databaseRef, (snapshot) => {
      const value = snapshot.val() || {};

      setData(value);

      const temperature = Number(value?.sensors?.temperature);
      const humidity = Number(value?.sensors?.humidity);

      if (!Number.isNaN(temperature)) {
        setTemperatureHistory((previous) => {
          const next = [...previous, temperature];
          return next.slice(-20);
        });
      }

      if (!Number.isNaN(humidity)) {
        setHumidityHistory((previous) => {
          const next = [...previous, humidity];
          return next.slice(-20);
        });
      }
    });

    return () => unsubscribe();
  }, []);

  // ==========================================
  // CLOCK
  // ==========================================

  useEffect(() => {
    const interval = setInterval(() => {
      setTime(new Date());
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  // ==========================================
  // SENSOR DATA
  // ==========================================

  const sensors = data.sensors || {};

  const temperature = sensors.temperature ?? "--";
  const humidity = sensors.humidity ?? "--";

  const rain = sensors.rainStatus ?? "Unknown";
  const rainValue = sensors.rainValue ?? "--";

  const vibration = sensors.vibration ?? "Unknown";

  const distance = sensors.distance ?? "--";

  const voltage = Number(sensors.voltage);

  const bagStatus = sensors.bagStatus ?? "Unknown";
  const bagOpen = bagStatus === "OPEN";

  const rfid = sensors.rfid ?? "None";

  const latitude = Number(sensors.latitude);
  const longitude = Number(sensors.longitude);

  const satellites = sensors.satellites ?? 0;

  // ==========================================
  // GPS
  // ==========================================

  const hasLocation =
    !Number.isNaN(latitude) &&
    !Number.isNaN(longitude) &&
    !(latitude === 0 && longitude === 0);

  const mapsUrl = hasLocation
    ? `https://www.google.com/maps?q=${latitude},${longitude}`
    : "#";

  // ==========================================
  // SECURITY
  // ==========================================

  const vibrationDetected =
    String(vibration).toUpperCase() === "DETECTED";

  const alarm = bagOpen || vibrationDetected;

  // ==========================================
  // BATTERY
  // ==========================================

  /*
    IMPORTANT:
    At the moment the voltage sensor is measuring
    the Arduino 5V test supply.

    This percentage is only a temporary display.
    Real battery calibration will be done later.
  */

  const batteryVoltage = Number.isNaN(voltage) ? 0 : voltage;

  const batteryValue =
    batteryVoltage <= 0
      ? 0
      : Math.min(
          Math.max(
            Math.round(
              ((batteryVoltage - 3.0) / (4.2 - 3.0)) * 100
            ),
            0
          ),
          100
        );

  const getBatteryStatus = () => {
    if (batteryValue <= 20) return "Low Battery";
    if (batteryValue <= 50) return "Medium";
    return "Good";
  };

  // ==========================================
  // STATUS FUNCTIONS
  // ==========================================

  const getRainStatus = () => {
    return rain;
  };

  const getVibrationStatus = () => {
    return vibration;
  };

  // ==========================================
  // UI
  // ==========================================

  return (
    <div className="app">

      {/* ================= SIDEBAR ================= */}

      <aside className="sidebar">

        <div className="logo-area">
          <div className="logo-box">🎒</div>

          <div>
            <h2>SmartPack</h2>
            <p>IoT COMMAND CENTER</p>
          </div>
        </div>

        <div className="nav-section">

          <p className="nav-title">MAIN MENU</p>

          <a className="nav-link active" href="#dashboard">
            <span>⌂</span>
            <strong>Dashboard</strong>
          </a>

          <a className="nav-link" href="#environment">
            <span>◉</span>
            <strong>Environment</strong>
          </a>

          <a className="nav-link" href="#security">
            <span>◈</span>
            <strong>Security</strong>
          </a>

          <a className="nav-link" href="#location">
            <span>⌖</span>
            <strong>GPS Tracking</strong>
          </a>

        </div>

        <div className="nav-section second">

          <p className="nav-title">SYSTEM</p>

          <a className="nav-link" href="#devices">
            <span>⌁</span>
            <strong>Devices</strong>
          </a>

          <a className="nav-link" href="#alerts">
            <span>♢</span>
            <strong>Alerts</strong>

            {alarm && (
              <span className="notification-dot"></span>
            )}
          </a>

        </div>

        <div className="sidebar-footer">

          <div className="connection-card">

            <div className="connection-icon">
              <span></span>
            </div>

            <div>
              <strong>System Online</strong>
              <p>Firebase Realtime</p>
            </div>

          </div>

          <div className="project-label">
            <strong>GROUP 04</strong>
            <span>Smart Backpack Project</span>
          </div>

        </div>

      </aside>

      {/* ================= MAIN ================= */}

      <main className="main">

        {/* TOP BAR */}

        <header className="topbar" id="dashboard">

          <div>

            <p className="eyebrow">
              SMART BACKPACK / COMMAND CENTER
            </p>

            <h1>Dashboard</h1>

          </div>

          <div className="top-actions">

            <div className="live-indicator">
              <span></span>
              LIVE MONITORING
            </div>

            <div className="time">
              {time.toLocaleTimeString()}
            </div>

            <div className="user-profile">

              <div className="avatar">
                SB
              </div>

              <div>
                <strong>Smart Backpack</strong>
                <span>Administrator</span>
              </div>

            </div>

          </div>

        </header>

        {/* ================= HERO ================= */}

        <section className="hero-card">

          <div className="hero-background-circle"></div>

          <div className="hero-left">

            <div className="backpack-visual">

              <div className="backpack-glow"></div>

              <div className="backpack-icon">
                🎒
              </div>

            </div>

            <div className="hero-text">

              <p>PROTECTION STATUS</p>

              <h2>
                {alarm
                  ? "Security Alert Detected"
                  : "Your Backpack is Protected"}
              </h2>

              <span>
                Real-time monitoring of security,
                environment and location.
              </span>

            </div>

          </div>

          <div
            className={
              alarm
                ? "hero-status alert"
                : "hero-status"
            }
          >

            <div className="status-icon">
              {alarm ? "!" : "✓"}
            </div>

            <div>

              <strong>
                {alarm
                  ? "SECURITY ALERT"
                  : "ALL SYSTEMS NORMAL"}
              </strong>

              <span>
                {alarm
                  ? "Immediate attention required"
                  : "Backpack is being monitored"}
              </span>

            </div>

          </div>

        </section>

        {/* ================= ENVIRONMENT ================= */}

        <section id="environment">

          <div className="section-heading">

            <div>
              <p>REAL-TIME DATA</p>
              <h2>Environment Overview</h2>
            </div>

            <span className="refresh-status">
              ● Updated live
            </span>

          </div>

          <div className="metric-grid">

            {/* Temperature */}

            <div className="metric-card">

              <div className="metric-header">

                <div className="metric-icon temperature-icon">
                  🌡️
                </div>

                <span>01</span>

              </div>

              <p>TEMPERATURE</p>

              <div className="metric-number">
                {temperature}
                <small>°C</small>
              </div>

              <div className="metric-bottom">
                <span>Current reading</span>
                <b>●</b>
              </div>

            </div>

            {/* Humidity */}

            <div className="metric-card">

              <div className="metric-header">

                <div className="metric-icon humidity-icon">
                  💧
                </div>

                <span>02</span>

              </div>

              <p>HUMIDITY</p>

              <div className="metric-number">
                {humidity}
                <small>%</small>
              </div>

              <div className="metric-bottom">
                <span>Air moisture</span>
                <b>●</b>
              </div>

            </div>

            {/* Rain */}

            <div className="metric-card">

              <div className="metric-header">

                <div className="metric-icon rain-icon">
                  🌧️
                </div>

                <span>03</span>

              </div>

              <p>RAIN SENSOR</p>

              <div className="metric-text">
                {getRainStatus()}
              </div>

              <div className="metric-bottom">
                <span>
                  Raw value: {rainValue}
                </span>
                <b>●</b>
              </div>

            </div>

            {/* Vibration */}

            <div className="metric-card">

              <div className="metric-header">

                <div className="metric-icon vibration-icon">
                  📳
                </div>

                <span>04</span>

              </div>

              <p>VIBRATION</p>

              <div className="metric-text">
                {getVibrationStatus()}
              </div>

              <div className="metric-bottom">
                <span>Movement detection</span>
                <b>●</b>
              </div>

            </div>

            {/* Battery */}

            <div className="metric-card battery-card">

              <div className="metric-header">

                <div className="metric-icon battery-icon">
                  🔋
                </div>

                <span>05</span>

              </div>

              <p>POWER / VOLTAGE</p>

              <div className="battery-content">

                <div
                  className="battery-ring"
                  style={{
                    "--battery": `${batteryValue * 3.6}deg`
                  }}
                >

                  <div>
                    <strong>
                      {batteryValue}
                    </strong>
                    <span>%</span>
                  </div>

                </div>

                <div>

                  <strong className="battery-status">
                    {getBatteryStatus()}
                  </strong>

                  <span className="battery-description">
                    {batteryVoltage
                      ? `${batteryVoltage.toFixed(2)} V`
                      : "-- V"}
                  </span>

                </div>

              </div>

            </div>

          </div>

        </section>

        {/* ================= EXTRA SENSOR DATA ================= */}

        <section>

          <div className="section-heading">

            <div>
              <p>SMART MONITORING</p>
              <h2>Backpack Sensor Status</h2>
            </div>

          </div>

          <div className="device-grid">

            <Device
              icon="📏"
              name="Ultrasonic Distance"
              status={`${distance} cm`}
              description="Distance monitoring"
            />

            <Device
              icon="🎒"
              name="Bag Status"
              status={bagStatus}
              description="Magnetic reed switch"
            />

            <Device
              icon="🔐"
              name="RFID"
              status={rfid}
              description="Last scanned RFID UID"
            />

            <Device
              icon="🛰️"
              name="GPS Satellites"
              status={`${satellites}`}
              description={
                hasLocation
                  ? "GPS fix acquired"
                  : "Waiting for GPS fix"
              }
            />

          </div>

        </section>

        {/* ================= ANALYTICS ================= */}

        <section className="analytics-grid">

          <div className="analytics-card">

            <div className="card-heading">

              <div>
                <p>LIVE ANALYTICS</p>
                <h2>Environmental Trends</h2>
              </div>

              <span className="chart-label">
                LAST 20 READINGS
              </span>

            </div>

            <div className="chart-area">

              <div className="chart-lines">
                <span></span>
                <span></span>
                <span></span>
                <span></span>
              </div>

              <div className="chart-bars">

                {temperatureHistory.length > 0
                  ? temperatureHistory.map(
                      (value, index) => {

                        const min = Math.min(
                          ...temperatureHistory
                        );

                        const max = Math.max(
                          ...temperatureHistory
                        );

                        const range =
                          max - min || 1;

                        const height =
                          25 +
                          ((value - min) / range) *
                            65;

                        return (
                          <div
                            className="chart-column"
                            key={index}
                          >

                            <div
                              className="chart-bar"
                              style={{
                                height: `${height}%`
                              }}
                            ></div>

                          </div>
                        );
                      }
                    )
                  : Array.from({
                      length: 20
                    }).map((_, index) => (

                      <div
                        className="chart-column"
                        key={index}
                      >

                        <div
                          className="chart-bar empty"
                          style={{
                            height:
                              `${25 +
                                (index % 5) * 8}%`
                          }}
                        ></div>

                      </div>

                    ))}

              </div>

            </div>

            <div className="chart-footer">

              <div>
                <span className="chart-dot"></span>
                Temperature
              </div>

              <strong>
                {temperature}°C
              </strong>

            </div>

          </div>

          {/* ================= SECURITY ================= */}

          <div
            className="security-card"
            id="security"
          >

            <div className="card-heading">

              <div>
                <p>SECURITY MONITOR</p>
                <h2>Protection Center</h2>
              </div>

              <div
                className={
                  alarm
                    ? "security-status danger"
                    : "security-status"
                }
              >
                {alarm ? "ALERT" : "SECURE"}
              </div>

            </div>

            <div className="security-main">

              <div
                className={
                  alarm
                    ? "security-shield danger-shield"
                    : "security-shield"
                }
              >
                {alarm ? "!" : "✓"}
              </div>

              <div>

                <h3>
                  {alarm
                    ? "Alarm Active"
                    : "Security Protected"}
                </h3>

                <p>
                  {alarm
                    ? "Unauthorized activity detected."
                    : "No security threats detected."}
                </p>

              </div>

            </div>

            <div className="security-items">

              <div>

                <span>🔐</span>

                <div>
                  <strong>RFID</strong>
                  <small>{rfid}</small>
                </div>

              </div>

              <div>

                <span>🎒</span>

                <div>
                  <strong>Bag</strong>
                  <small>{bagStatus}</small>
                </div>

              </div>

              <div>

                <span>📳</span>

                <div>
                  <strong>Vibration</strong>
                  <small>
                    {getVibrationStatus()}
                  </small>
                </div>

              </div>

            </div>

          </div>

        </section>

        {/* ================= GPS ================= */}

        <section
          className="location-section"
          id="location"
        >

          <div className="section-heading">

            <div>
              <p>GPS TRACKING</p>
              <h2>Live Backpack Location</h2>
            </div>

            <div className="gps-online">
              <span></span>

              {hasLocation
                ? `GPS FIX • ${satellites} SAT`
                : `WAITING • ${satellites} SAT`}
            </div>

          </div>

          <div className="location-card">

            <div className="fake-map">

              <div className="map-grid"></div>

              <div className="map-road road-one"></div>
              <div className="map-road road-two"></div>
              <div className="map-road road-three"></div>

              <div className="map-location">

                <div className="location-pulse"></div>

                <div className="location-pin">
                  📍
                </div>

                <div className="location-label">

                  <strong>
                    BACKPACK
                  </strong>

                  <span>
                    {hasLocation
                      ? "Current GPS position"
                      : "Waiting for satellite fix"}
                  </span>

                </div>

              </div>

            </div>

            <div className="location-info">

              <div className="coordinates-card">

                <div className="coordinate">

                  <span>LATITUDE</span>

                  <strong>
                    {hasLocation
                      ? latitude.toFixed(6)
                      : "--"}
                  </strong>

                </div>

                <div className="coordinate">

                  <span>LONGITUDE</span>

                  <strong>
                    {hasLocation
                      ? longitude.toFixed(6)
                      : "--"}
                  </strong>

                </div>

              </div>

              <div className="gps-status-box">

                <div className="gps-icon">
                  ⌖
                </div>

                <div>

                  <strong>
                    GPS Location
                  </strong>

                  <span>
                    {hasLocation
                      ? `Coordinates received • ${satellites} satellites`
                      : `Waiting for GPS fix • ${satellites} satellites`}
                  </span>

                </div>

              </div>

              <a
                href={mapsUrl}
                target="_blank"
                rel="noreferrer"
                onClick={(event) => {
                  if (!hasLocation) {
                    event.preventDefault();
                  }
                }}
                className={
                  hasLocation
                    ? "maps-button"
                    : "maps-button disabled"
                }
              >

                View on Google Maps
                <span>↗</span>

              </a>

            </div>

          </div>

        </section>

        {/* ================= DEVICES ================= */}

        <section id="devices">

          <div className="section-heading">

            <div>
              <p>HARDWARE</p>
              <h2>Device Status</h2>
            </div>

          </div>

          <div className="device-grid">

            <Device
              icon="⚡"
              name="Arduino UNO R4 WiFi"
              status="Connected"
              description="Main controller"
            />

            <Device
              icon="📡"
              name="Firebase RTDB"
              status="Connected"
              description="Cloud database"
            />

            <Device
              icon="🌡️"
              name="DHT22 Sensor"
              status="Monitoring"
              description="Temperature & humidity"
            />

            <Device
              icon="🌧️"
              name="Rain Sensor"
              status={rain}
              description={`Raw reading: ${rainValue}`}
            />

            <Device
              icon="📳"
              name="Vibration Sensor"
              status={vibration}
              description="Anti-theft movement detection"
            />

            <Device
              icon="🎒"
              name="Reed Switch"
              status={bagStatus}
              description="Bag open/close detection"
            />

            <Device
              icon="📏"
              name="HC-SR04"
              status={`${distance} cm`}
              description="Ultrasonic distance sensor"
            />

            <Device
              icon="🔐"
              name="RC522 RFID"
              status={rfid}
              description="Owner identification"
            />

            <Device
              icon="📍"
              name="GPS Module"
              status={
                hasLocation
                  ? "Receiving"
                  : "Waiting"
              }
              description={`${satellites} satellite(s)`}
            />

            <Device
              icon="🔋"
              name="Voltage Sensor"
              status={
                batteryVoltage
                  ? `${batteryVoltage.toFixed(2)} V`
                  : "--"
              }
              description="Power monitoring"
            />

          </div>

        </section>

        {/* ================= ALERTS ================= */}

        <section
          className="alerts-section"
          id="alerts"
        >

          <div className="section-heading">

            <div>
              <p>EVENT MONITORING</p>
              <h2>Recent Activity</h2>
            </div>

          </div>

          <div className="activity-card">

            <Activity
              icon="☁"
              title="Firebase Connection"
              description="Realtime database connected"
              time="LIVE"
              success
            />

            <Activity
              icon="🔐"
              title="RFID Authentication"
              description={`Last RFID: ${rfid}`}
              time="LIVE"
              success={rfid !== "None"}
            />

            <Activity
              icon="🎒"
              title="Backpack Status"
              description={
                bagOpen
                  ? "Backpack is currently open"
                  : "Backpack is securely closed"
              }
              time="LIVE"
              success={!bagOpen}
            />

            <Activity
              icon="📳"
              title="Vibration Detection"
              description={
                vibrationDetected
                  ? "Movement detected"
                  : "No unusual movement detected"
              }
              time="LIVE"
              success={!vibrationDetected}
            />

            <Activity
              icon="🌧️"
              title="Rain Detection"
              description={
                rain === "RAIN"
                  ? "Rain / water detected"
                  : "No rain detected"
              }
              time="LIVE"
              success={rain !== "RAIN"}
            />

            <Activity
              icon="📍"
              title="GPS Tracking"
              description={
                hasLocation
                  ? `${latitude.toFixed(6)}, ${longitude.toFixed(6)}`
                  : `Waiting for GPS fix • ${satellites} satellites`
              }
              time="LIVE"
              success={hasLocation}
            />

          </div>

        </section>

        {/* ================= FOOTER ================= */}

        <footer className="footer">

          <div>
            <strong>SMARTPACK IoT</strong>
            <span>
              Smart Backpack Safety System
            </span>
          </div>

          <div>
            <span>
              MICROCONTROLLER SYSTEMS
            </span>

            <strong>
              GROUP 04
            </strong>
          </div>

        </footer>

      </main>

    </div>
  );
}

// ==========================================
// DEVICE COMPONENT
// ==========================================

function Device({
  icon,
  name,
  status,
  description
}) {
  return (
    <div className="device-card">

      <div className="device-icon">
        {icon}
      </div>

      <div className="device-details">
        <strong>{name}</strong>
        <span>{description}</span>
      </div>

      <div className="device-status">
        <span></span>
        {status}
      </div>

    </div>
  );
}

// ==========================================
// ACTIVITY COMPONENT
// ==========================================

function Activity({
  icon,
  title,
  description,
  time,
  success = true
}) {
  return (
    <div className="activity-row">

      <div
        className={
          success
            ? "activity-icon"
            : "activity-icon warning"
        }
      >
        {icon}
      </div>

      <div className="activity-content">
        <strong>{title}</strong>
        <span>{description}</span>
      </div>

      <div className="activity-time">
        {time}
      </div>

    </div>
  );
}

export default App;