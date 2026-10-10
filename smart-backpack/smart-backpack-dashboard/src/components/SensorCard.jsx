    function SensorCard({ icon, title, value }) {

    return (
        <div className="sensor-card">

        <div className="sensor-icon">
            {icon}
        </div>

        <div>

            <p>{title}</p>

            <h3>{value}</h3>

        </div>

        </div>
    );
    }

    export default SensorCard;