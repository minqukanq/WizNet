var mongoose = require('mongoose');
mongoose.Promise = global.Promise;
var schema = mongoose.Schema({

	PM25_NO : String,
	PM25_DATE : String,
	PM25_TIME : String,
	PM25_01 : Number,
	PM25_10 : Number,
	PM25_25 : Number,
	DS_TEMP : Number

});

module.exports = mongoose.model('pmSensor', schema);
