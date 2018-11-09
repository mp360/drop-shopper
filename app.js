const express = require('express');
const path = require('path');
const bodyParser = require('body-parser');
const cors = require('cors');
const passport = require('passport');
const mongoose = require('mongoose');
const config = require('./config/database')

const app = express();
const users = require('./routes/users');

// Connect to database
mongoose.connect(config.database);

// On connection 
mongoose.connection.on('connected', () => {
    console.log('connected to database '+config.database);
});
// On error
mongoose.connection.on('error', (err) => {
    console.log('Database error: '+err);
});

// port number
const port = 3000;

// CORS Middleware
app.use(cors());

// Set static folder
    // will hold all client-side code 
app.use(express.static(path.join(__dirname, 'client')));

// Body Parser middleware
app.use(bodyParser.json());

//Passport middleware
app.use(passport.initialize());
app.use(passport.session());
require('./config/passport')(passport); //include passport.js


app.use('/users', users);

// Index Route
app.get('/', (req,res) => {
    res.send('Invalid Endpoint');
});

// Start Server
app.listen(port, () => {
    console.log('Server started on port ' + port);
});
// ^^^ main server entry point module
