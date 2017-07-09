module.exports =(app,pm25VO)=>{
	/*
	 * 아두이노에서 서버에 데이터 insert 요청
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
	});
	
    app.get("/pm25/pmlist",(req,res)=>{
         res.render("list");
    });
    
    // 라인 그래프 
    app.get('/pm25/linechart',(req,res)=>{
        res.render('line_chart')
    })

    app.get('/pm25/chart/:getDate',(req,res)=>{
    	var searchdate = req.params.getDate; // ajax에서 넘긴 데이터는 query로 받는다
    	
    	pm25VO.find({PM25_DATE:searchdate})
    	.exec((err,data)=>{
    		
    			/**
    			 * javaScript의 filter Method를 이용해서 10분 단위로 데이터를 필터링한다.
    			 */
    			var oldTime = '';
    			var newData = data.filter(function(item){
    				var newTime =  item.PM25_TIME.substring(0,2)+item.PM25_TIME.substring(3,4)
    				if(oldTime != newTime) {
    					oldTime = newTime;
    					item.PM25_TIME = new Date(item.PM25_DATE +' ' + item.PM25_TIME).getTime()
    					console.log(new Date('시간값'+item.PM25_TIME))
    					
//    					item.PM25_TIME = item.PM25_TIME
    					return item;	
    				}
    				
    			});  
//    			console.log(newArr);
//    			var a1 = [1, 2, 3, 4, 5, 6]
//    			newData.slice(3).map(function(id){console.log(id)})
    			res.json(newData)
    		
    	})
    })
    
    
    // Ajax에서 조회할 리스트 데이터
    // 날짜값을 인자로 받아서 조회한다.
    app.post("/pm25/getlist",(req,res)=>{
    	
    	console.log(req.body.getDate);
    	var searchdate = req.body.getDate; // ajax에서 넘긴 데이터는 query로 받는다
        pm25VO
                .find({PM25_DATE:searchdate})
                .sort({PM25_DATE:-1})
                .sort({PM25_TIME:-1})
                .exec(function(err,data){
        			var oldTime = '';
        			var newData = data.filter(function(item){
        				if(oldTime != item.PM25_TIME.substring(0,4)) {
        					console.log(oldTime)
        					oldTime = item.PM25_TIME.substring(0,4);
        					return item;	
        				}
        			});  

                	
                	res.render("listbody",{list:newData,title:"/pm25/list"});
                });
    });
    
    // dashboard 페이지 로딩
	app.get("/pm25/dashboard",(req,res)=>{
		res.render("dashboard");
	});
	
	// WEB에서 GET으로 last 데이터를 get 한다.
	app.get("/pm25/getlast",(req,res)=>{

		Date.prototype.yyyymmdd = function(){
		    var yyyy = this.getFullYear().toString();
		    var mm = (this.getMonth() + 1).toString();
		    var dd = this.getDate().toString();
		    return yyyy +"-"+(mm[1] ? mm : '0'+mm[0])+"-"+(dd[1] ? dd : '0'+dd[0]);
		};
		 
		var nowDate = (new Date).yyyymmdd();
		console.log(nowDate);

		//		nowDate = '2017-06-15'
		//		pm25VO.find({PM25_DATE:nowDate})

		// 가장 마지막에 insert된 데이터 1개만 find()
		pm25VO.findOne()
			.sort({PM25_DATE:-1})
			.sort({PM25_TIME: -1})
			.exec((err,data)=>{
				res.json(data);
			});
	})

	
	// Date.prototype.yyyymmdd = function(){
	// var yyyy = this.getFullYear().toString();
	// var mm = (this.getMonth() + 1).toString();
	// var dd = this.getDate().toString();
	// return yyyy +"-"+(mm[1] ? mm : '0'+mm[0])+"-"+(dd[1] ? dd : '0'+dd[0]);
	// };
	//		 
	// var nowDate = (new Date).yyyymmdd();
	// console.log(nowDate);


	// 안드로이드 APP에서 최근 Data Get
	// 안드로이드 APP에서는 HttpReq로 질의를 하면 무조건 POST 질의가 된다.
	app.post("/pm25/getlast",(req,res)=>{
		pm25VO.findOne()
			.sort({PM25_DATE:-1})
			.sort({PM25_TIME: -1})
			.exec((err,data)=>{
				res.status(200);
				res.json(data);
		});
	})
	
	// Test Get 모든 Data List 불러오기
	// 서버에 과부하 우려로 주의
	/*
	app.get("/pm25/allist",(req,res)=>{
		pm25VO.find((err,data)=>{
			res.json(data);
		})
	})
	*/
	

	
	// 정보보기 페이지 로딩
	app.get("/pm25/getinfo",(req,res)=>{
		res.render("info");
	})
	
	
	
	
}