module.exports =(app,pm25VO)=>{
	app.get("/wifi/:msg",(req,res)=>{
		console.log(req.params.msg) ;
		res.status(200);
		res.end("")
	})
	
	/*
	 * PM25_NO : String, PM25_DATE : String, PM25_TIME : String, PM25_01 :
	 * Number, PM25_10 : Number, PM25_25 : Number
	 */
	app.get("/pm25/:PM25_NO/:PM25_DATE/:PM25_TIME/:PM25_01/:PM25_10/:PM25_25/:DS_TEMP",(req,res)=>{
		
		var pm25_01 = parseInt(req.params.PM25_01)
		var pm25_25 = parseInt(req.params.PM25_25)
		var pm25_10 = parseInt(req.params.PM25_10)
		
		// req 로 부터 접속자 IP 주소를 가져온다
		var req_ip = req.headers['x-forwarded-for']
					|| req.connection.remoteAddress || req.socket.remoteAddress
					|| req.connection.socket.remoteAddress;

		
		if(pm25_01 > 0 && pm25_25 > 0 && pm25_10) {
			var vo = new pm25VO(req.params);
			vo.save(function(err,data){
				res.status(200);
				res.send('ok');
			});
		}
		
// res.status(200);
// res.write(req_ip+"\n");
// res.write(req.params.PM25_DATE+"\n");
// res.write(req.params.PM25_TIME+"\n");
// res.write(req.params.PM25_01+"\n");
// res.write(req.params.PM25_10+"\n");
// res.write(req.params.PM25_25+"\n");
// res.end(req.params.DS_TEMP+"\n");

	});
	
    app.get("/pm25/pmlist",(req,res)=>{
         res.render("list");
    });

    app.get("/pm25/getlist",(req,res)=>{
    	
    	console.log(req.query.searchdate);
    	var searchdate = req.query.searchdate; // ajax에서 넘긴 데이터는 query로 받는다
        pm25VO
                .find({PM25_DATE:searchdate})
                .sort({PM25_DATE:-1})
                .sort({PM25_TIME:-1})
                .exec(function(err,data){
                res.render("listbody",{list:data,title:"/pm25/list"});
        });
});

    
	app.get("/pm25/monthview",(req,res)=>{
		pm25VO.find((err,data)=>{
			res.render("monthview");
		});
	});
	app.get("/pm25/dashboard",(req,res)=>{
		res.render("dashboard");
	});
	app.get("/pm25/getlast",(req,res)=>{

		Date.prototype.yyyymmdd = function(){
		    var yyyy = this.getFullYear().toString();
		    var mm = (this.getMonth() + 1).toString();
		    var dd = this.getDate().toString();
		    return yyyy +"-"+(mm[1] ? mm : '0'+mm[0])+"-"+(dd[1] ? dd : '0'+dd[0]);
		};
		 
		var nowDate = (new Date).yyyymmdd();
		console.log(nowDate);

		nowDate = '2017-06-15'
// pm25VO.find({PM25_DATE:nowDate})

		pm25VO.findOne()
			.sort({PM25_DATE:-1})
			.sort({PM25_TIME: -1})
			.exec((err,data)=>{
				res.json(data);
		});
	})
	
	app.post("/pm25/getlast",(req,res)=>{

// Date.prototype.yyyymmdd = function(){
// var yyyy = this.getFullYear().toString();
// var mm = (this.getMonth() + 1).toString();
// var dd = this.getDate().toString();
// return yyyy +"-"+(mm[1] ? mm : '0'+mm[0])+"-"+(dd[1] ? dd : '0'+dd[0]);
// };
//		 
// var nowDate = (new Date).yyyymmdd();
// console.log(nowDate);

		pm25VO.findOne()
			.sort({PM25_DATE:-1})
			.sort({PM25_TIME: -1})
			.exec((err,data)=>{
				res.status(200);
				res.json(data);
		});
	})
	
	app.get("/pm25/allist",(req,res)=>{
		pm25VO.find((err,data)=>{
			res.json(data);
		})
	})
	
	app.get("/pm25/getinfo",(req,res)=>{
		res.render("info");
	})
	
	
	
	
}