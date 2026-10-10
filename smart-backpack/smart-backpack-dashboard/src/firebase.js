    import { initializeApp } from "firebase/app";
    import { getDatabase } from "firebase/database";

    const firebaseConfig = {
    apiKey: "AIzaSyCCgyfovVYps1is2cIlI6djV2aHC2g4kfY",
    authDomain: "smart-backpack-iot.firebaseapp.com",
    databaseURL: "https://smart-backpack-iot-default-rtdb.firebaseio.com",
    projectId: "smart-backpack-iot",
    storageBucket: "smart-backpack-iot.firebasestorage.app",
    messagingSenderId: "483440317294",
    appId: "1:483440317294:web:7c271199c011deb8e1677f",
    };

    const app = initializeApp(firebaseConfig);

    export const database = getDatabase(app);