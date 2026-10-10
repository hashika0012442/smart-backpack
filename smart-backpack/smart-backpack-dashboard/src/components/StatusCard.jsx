    function StatusCard({ icon, title, value, status }) {

    return (
        <div className={`status-card ${status}`}>

        <div className="card-icon">
            {icon}
        </div>

        <div>

            <p>{title}</p>

            <h3>{value}</h3>

        </div>

        </div>
    );
    }

    export default StatusCard;