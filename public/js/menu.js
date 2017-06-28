	$(document).ready(function() {
		createGauges();
		updateGauges();
		setInterval(updateGauges, 10000);
		
		$("#dashboard").click(()=>{
			location.href="/pm25/dashboard";
		})
		$("#pmlist").click(()=>{
			location.href="/pm25/pmlist";
		})
		$("#getinfo").click(()=>{
			location.href="/pm25/getinfo";
		})
	})