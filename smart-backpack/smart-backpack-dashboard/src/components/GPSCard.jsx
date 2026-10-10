    function GPSCard({ latitude, longitude }) {

    return (
        <div className="gps-card">

        <div className="gps-info">

            <div className="gps-icon">
            📍
            </div>

            <div>

            <p>Current Location</p>

            <h3>
                {latitude}, {longitude}
            </h3>

            <span>
                GPS Signal Active
            </span>

            </div>

        </div>

        <div className="map-placeholder">

            <div className="map-pin">
            📍
            </div>

            <p>
            GPS Location Map
            </p>

        </div>

        </div>
    );
    }

    export default GPSCard;