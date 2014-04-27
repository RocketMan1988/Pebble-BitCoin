var symbol = localStorage.getItem("symbol");

// We use the fake "PBL" symbol as default
if (!symbol) {
  symbol = "DDD";
}

// Fetch bitcoin data
// & send back to the watch via app message
// API documentation at http://dev.markitondemand.com/#doc
function fetchStockQuote(symbol) {
  var response;
  var req = new XMLHttpRequest();
  // build the GET request
  req.open('GET', "http://api.coindesk.com/v1/bpi/currentprice.json", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      // 200 - HTTP OK
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var price;
        if (response.bpi) {
          // data found, look for rate
          if (symbol == "USD: ")
          {
          rate = response.bpi.USD.rate;
          console.log(symbol);
          console.log(rate);
          Pebble.sendAppMessage({
            "1": rate.toString()});
	      }
	      if (symbol == "EUR: ")
          {
          rate = response.bpi.EUR.rate;
          console.log(symbol);
          console.log(rate);
          Pebble.sendAppMessage({
            "1": rate.toString()});
	      }
	      if (symbol == "GBP: ")
          {
          rate = response.bpi.GBP.rate;
          console.log(symbol);
          console.log(rate);
          Pebble.sendAppMessage({
            "1": rate.toString()});
	      }
        }
        else {
			console.log("No bpi found...");
		}
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  }
  req.send(null);
}

    
  Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");
  }
);  

Pebble.addEventListener("appmessage",
  function(e) {
	symbol = e.payload.symbol;  
    if (symbol == "USD: ") {
    console.log("Received message(USD): " + e.payload.symbol);
	}
    
    fetchStockQuote(symbol);
    console.log("Received message: " + e.payload);

  }
);
