/**
 * @Version 1.0
 * @Author Li Yao
 * @Since 2011-07-02
 */

/*******************************************************************************
 * @Class StringBuilder
 ******************************************************************************/
/**
 * Initializes a new instance of the StringBuilder class and appends the given
 * value if supplied.
 *
 * @param value
 */
function StringBuilder(value) {
	this.strings = new Array("");
	this.append(value);
};

/**
 * Appends the given value to the end of this instance.
 *
 * @param value
 */
StringBuilder.prototype.append = function(value) {
	if (value) {
		this.strings.push(value);
	}
};

/**
 * Clears the string buffer.
 */
StringBuilder.prototype.clear = function() {
	this.strings.length = 1;
};

/**
 * Converts this instance to a String.
 */
StringBuilder.prototype.toString = function() {
	return this.strings.join("");
};

/*******************************************************************************
 * @Class String
 ******************************************************************************/
String.format = function() {
	if (arguments.length == 0)
		return null;

	var s = arguments[0];
	for ( var i = 0; i < arguments.length - 1; i++) {
		var reg = new RegExp("\\{" + i + "\\}", "gm");
		s = s.replace(reg, arguments[i + 1]);
	}

	return s;
};

String.prototype.format = function() {
	var args = arguments;
	return this.replace(/\{(\d+)\}/g, function(m, i) {
		return args[i];
	});
};

String.prototype.trim = function() {
	return this.replace(/(^\s*)|(\s*$)/g, "");
};

String.prototype.replaceSingleQuote = function() {
	return this.replace(/\'/g, "");
};

String.prototype.replaceQuote = function() {
	return this.replace(/[\'|\"]/g, "");
};

String.prototype.endsWith = function(suffix) {
	return (this.substr(this.length - suffix.length) === suffix);
};

String.prototype.startsWith = function(prefix) {
	return (this.substr(0, prefix.length) === prefix);
};

String.getUtf8Length = function(value) {
	return value.replace(/[^\x00-\xff]/g, "***").length;
};

/*******************************************************************************
 * @Class 处理长整形
 ******************************************************************************/
function DoubleInt(num) {
	if (!num) num = "0";

	// 负数转化为正
	if (num[0] == "-") {
		this.sign = -1;
		num = num.substring(1);
	} else {
		this.sign = 1;
	}

	var highNum = "0", lowNum = num;
	var len = num.length;
	if (len > 8) {
		highNum = num.substring(0, len - 8);
		lowNum = num.substring(len - 8);
	}
	this.HIGH_BASE = 100000000;
	this.high = parseInt(highNum, 10);
	this.low = parseInt(lowNum, 10);
}

DoubleInt.prototype.toPositive = function() {
	this.sign = 1;
};

DoubleInt.prototype.toNegetive = function() {
	this.sign = -1;
};

DoubleInt.prototype.add = function(num) {
	var sum = new DoubleInt();
	if (!(num instanceof DoubleInt)) {
		return sum;
	}
	var num1 = this, num2 = num;

	var highSum, lowSum;
	// 两个正数和两个负数相加
	if (num1.sign == num2.sign) {
		sum.high = num1.high + num2.high;
		lowSum = num1.low + num2.low;
		if (lowSum >= sum.HIGH_BASE) {
			sum.low = lowSum % sum.HIGH_BASE;
			sum.high += 1;
		} else {
			sum.low = lowSum;
		}
	} else {
		highSum = num1.high * num1.sign + num2.high * num2.sign;
		lowSum = num1.low * num1.sign + num2.low * num2.sign;
		if (highSum == 0) {
			sum.high = 0;
			if (lowSum >= 0) {
				sum.sign = 1;
				sum.low = lowSum;
			} else {
				sum.sign = -1;
				sum.low = Math.abs(lowSum);
			}
		} else if (highSum > 0){
			sum.sign = 1;
			sum.high = highSum;
			if (lowSum >= 0){
				sum.low = lowSum;
			} else {
				sum.low = sum.HIGH_BASE + lowSum;
				sum.high -= 1;
			}
		} else {
			sum.sign = -1;
			sum.high = Math.abs(highSum);
			if (lowSum <= 0){
				sum.low = Math.abs(lowSum);
			} else {
				sum.low = sum.HIGH_BASE - lowSum;
				sum.high -= 1;
			}
		}
	}

	return sum;
};

/**
 * 数字和DoubleInt乘法
 * @returns
 */
DoubleInt.prototype.multiply = function (num) {
};

DoubleInt.prototype.toString = function () {
	var lowNum = ("00000000" + this.low);
	return String.format("{0}{1}{2}",
			this.sign >= 0 ? "" : "-",
			this.high == 0 ? "" : this.high,
			this.high == 0 ? this.low : lowNum.substring(lowNum.length - 8));
};

/*******************************************************************************
 * @Class Uri 匹配http或https
 ******************************************************************************/
function Uri(url) {
	this.url = url;
	var reg = /^(http|https)\:\/\/([\w\d\.]+)(?:\:(\d+))?(\/?(?:.*))$/;
	if (reg.test(url)) {
		this.protocol = RegExp.$1;
		this.domain = RegExp.$2;
		this.port = RegExp.$3;
		this.path = RegExp.$4;
	}
};

Uri.prototype.getUrl = function() {
	return this.url;
};

Uri.prototype.getProtocol = function() {
	return this.protocol;
};

Uri.prototype.getDomain = function() {
	return this.domain;
};

Uri.prototype.getPort = function() {
	return this.port;
};

Uri.prototype.getPath = function() {
	return this.path;
};

var Md5 = {
	hexKey : "0123456789abcdef",
	rhex : function(num) {
		str = "";
		for ( var j = 0; j <= 3; j++) {
			str += Md5.hexKey.charAt((num >> (j * 8 + 4)) & 15) + Md5.hexKey.charAt((num >> (j * 8)) & 15);
		}
		return str;
	},
	str2blks_MD5 : function(str) {
		nblk = ((str.length + 8) >> 6) + 1;
		blks = new Array(nblk * 16);
		for ( var i = 0; i < nblk * 16; i++) {
			blks[i] = 0;
		}
		for ( var i = 0; i < str.length; i++) {
			blks[i >> 2] |= str.charCodeAt(i) << ((i % 4) * 8);
		}
		blks[i >> 2] |= 128 << ((i % 4) * 8);
		blks[nblk * 16 - 2] = str.length * 8;
		return blks;
	},
	add : function(x, y) {
		var lsw = (x & 65535) + (y & 65535);
		var msw = (x >> 16) + (y >> 16) + (lsw >> 16);
		return (msw << 16) | (lsw & 65535);
	},
	rol : function(num, cnt) {
		return (num << cnt) | (num >>> (32 - cnt));
	},
	cmn : function(q, a, b, x, s, t) {
		return Md5.add(Md5.rol(Md5.add(Md5.add(a, q), Md5.add(x, t)), s), b);
	},
	ff : function(a, b, c, d, x, s, t) {
		return Md5.cmn((b & c) | ((~b) & d), a, b, x, s, t);
	},
	gg : function(a, b, c, d, x, s, t) {
		return Md5.cmn((b & d) | (c & (~d)), a, b, x, s, t);
	},
	hh : function(a, b, c, d, x, s, t) {
		return Md5.cmn(b ^ c ^ d, a, b, x, s, t);
	},
	ii : function(a, b, c, d, x, s, t) {
		return Md5.cmn(c ^ (b | (~d)), a, b, x, s, t);
	},
	md5 : function(str) {
		x = Md5.str2blks_MD5(str);
		var a = 1732584193;
		var b = -271733879;
		var c = -1732584194;
		var d = 271733878;
		for ( var i = 0; i < x.length; i += 16) {
			var olda = a;
			var oldb = b;
			var oldc = c;
			var oldd = d;
			a = Md5.ff(a, b, c, d, x[i + 0], 7, -680876936);
			d = Md5.ff(d, a, b, c, x[i + 1], 12, -389564586);
			c = Md5.ff(c, d, a, b, x[i + 2], 17, 606105819);
			b = Md5.ff(b, c, d, a, x[i + 3], 22, -1044525330);
			a = Md5.ff(a, b, c, d, x[i + 4], 7, -176418897);
			d = Md5.ff(d, a, b, c, x[i + 5], 12, 1200080426);
			c = Md5.ff(c, d, a, b, x[i + 6], 17, -1473231341);
			b = Md5.ff(b, c, d, a, x[i + 7], 22, -45705983);
			a = Md5.ff(a, b, c, d, x[i + 8], 7, 1770035416);
			d = Md5.ff(d, a, b, c, x[i + 9], 12, -1958414417);
			c = Md5.ff(c, d, a, b, x[i + 10], 17, -42063);
			b = Md5.ff(b, c, d, a, x[i + 11], 22, -1990404162);
			a = Md5.ff(a, b, c, d, x[i + 12], 7, 1804603682);
			d = Md5.ff(d, a, b, c, x[i + 13], 12, -40341101);
			c = Md5.ff(c, d, a, b, x[i + 14], 17, -1502002290);
			b = Md5.ff(b, c, d, a, x[i + 15], 22, 1236535329);
			a = Md5.gg(a, b, c, d, x[i + 1], 5, -165796510);
			d = Md5.gg(d, a, b, c, x[i + 6], 9, -1069501632);
			c = Md5.gg(c, d, a, b, x[i + 11], 14, 643717713);
			b = Md5.gg(b, c, d, a, x[i + 0], 20, -373897302);
			a = Md5.gg(a, b, c, d, x[i + 5], 5, -701558691);
			d = Md5.gg(d, a, b, c, x[i + 10], 9, 38016083);
			c = Md5.gg(c, d, a, b, x[i + 15], 14, -660478335);
			b = Md5.gg(b, c, d, a, x[i + 4], 20, -405537848);
			a = Md5.gg(a, b, c, d, x[i + 9], 5, 568446438);
			d = Md5.gg(d, a, b, c, x[i + 14], 9, -1019803690);
			c = Md5.gg(c, d, a, b, x[i + 3], 14, -187363961);
			b = Md5.gg(b, c, d, a, x[i + 8], 20, 1163531501);
			a = Md5.gg(a, b, c, d, x[i + 13], 5, -1444681467);
			d = Md5.gg(d, a, b, c, x[i + 2], 9, -51403784);
			c = Md5.gg(c, d, a, b, x[i + 7], 14, 1735328473);
			b = Md5.gg(b, c, d, a, x[i + 12], 20, -1926607734);
			a = Md5.hh(a, b, c, d, x[i + 5], 4, -378558);
			d = Md5.hh(d, a, b, c, x[i + 8], 11, -2022574463);
			c = Md5.hh(c, d, a, b, x[i + 11], 16, 1839030562);
			b = Md5.hh(b, c, d, a, x[i + 14], 23, -35309556);
			a = Md5.hh(a, b, c, d, x[i + 1], 4, -1530992060);
			d = Md5.hh(d, a, b, c, x[i + 4], 11, 1272893353);
			c = Md5.hh(c, d, a, b, x[i + 7], 16, -155497632);
			b = Md5.hh(b, c, d, a, x[i + 10], 23, -1094730640);
			a = Md5.hh(a, b, c, d, x[i + 13], 4, 681279174);
			d = Md5.hh(d, a, b, c, x[i + 0], 11, -358537222);
			c = Md5.hh(c, d, a, b, x[i + 3], 16, -722521979);
			b = Md5.hh(b, c, d, a, x[i + 6], 23, 76029189);
			a = Md5.hh(a, b, c, d, x[i + 9], 4, -640364487);
			d = Md5.hh(d, a, b, c, x[i + 12], 11, -421815835);
			c = Md5.hh(c, d, a, b, x[i + 15], 16, 530742520);
			b = Md5.hh(b, c, d, a, x[i + 2], 23, -995338651);
			a = Md5.ii(a, b, c, d, x[i + 0], 6, -198630844);
			d = Md5.ii(d, a, b, c, x[i + 7], 10, 1126891415);
			c = Md5.ii(c, d, a, b, x[i + 14], 15, -1416354905);
			b = Md5.ii(b, c, d, a, x[i + 5], 21, -57434055);
			a = Md5.ii(a, b, c, d, x[i + 12], 6, 1700485571);
			d = Md5.ii(d, a, b, c, x[i + 3], 10, -1894986606);
			c = Md5.ii(c, d, a, b, x[i + 10], 15, -1051523);
			b = Md5.ii(b, c, d, a, x[i + 1], 21, -2054922799);
			a = Md5.ii(a, b, c, d, x[i + 8], 6, 1873313359);
			d = Md5.ii(d, a, b, c, x[i + 15], 10, -30611744);
			c = Md5.ii(c, d, a, b, x[i + 6], 15, -1560198380);
			b = Md5.ii(b, c, d, a, x[i + 13], 21, 1309151649);
			a = Md5.ii(a, b, c, d, x[i + 4], 6, -145523070);
			d = Md5.ii(d, a, b, c, x[i + 11], 10, -1120210379);
			c = Md5.ii(c, d, a, b, x[i + 2], 15, 718787259);
			b = Md5.ii(b, c, d, a, x[i + 9], 21, -343485551);
			a = Md5.add(a, olda);
			b = Md5.add(b, oldb);
			c = Md5.add(c, oldc);
			d = Md5.add(d, oldd);
		}
		return Md5.rhex(a) + Md5.rhex(b) + Md5.rhex(c) + Md5.rhex(d);
	}
};

/*******************************************************************************
 * @Class JSON
 ******************************************************************************/
var JSON = JSON || {};
// implement JSON.stringify serialization
JSON.stringify = JSON.stringify || function(obj) {
	var t = typeof (obj);
	if (t != "object" || obj === null) {
		// simple data type
		if (t == "string")
			obj = '"' + obj + '"';
		return String(obj);
	} else {
		// recurse array or object
		var v, json = [], arr = (obj && obj.constructor == Array);
		for ( var n in obj) {
			v = obj[n];
			t = typeof (v);
			if (t == "string")
				v = '"' + v + '"';
			else if (t == "object" && v !== null)
				v = JSON.stringify(v);
			json.push((arr ? "" : '"' + n + '":') + String(v));
		}
		return (arr ? "[" : "{") + String(json) + (arr ? "]" : "}");
	}
};
// implement JSON.parse de-serialization
JSON.parse = JSON.parse || function(str) {
	if (str === "") {
		str = '""';
	}

	eval("var p=" + str + ";");
	return p;
};

/**
 * 解析JSON对象，options = { hasDate: true } 时，可以解析如下日期格式yyyy-MM-ddTHH:mm:ssZ
 *
 * @param str
 * @param options
 * @returns
 */
JSON.richParse = function(str, options) {
	if (str === "") {
		str = '""';
	} else {
		if (options) {
			var opts = $.extend({}, JSON.richParse.defaultOptions, options);
			// 解析时间类型
			if (opts.hasDate) {
				var reg = /(\"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}.\d{3}Z\")/g;
				str = str.replace(reg, "new Date(Date.parse($1))");
			}
		}
	}

	eval("var p=" + str + ";");
	return p;
};

JSON.richParse.defaultOptions = {
	hasDate : false
};

/**
 * 将json对象序列化为参数
 */
JSON.serializeToParms = function(json) {
	var sb = new StringBuilder();
	var first = true;
	for (p in json) {
		if (!first) {
			sb.append("&");
		} else {
			first = false;
		}
		sb.append(p);
		sb.append("=");
		sb.append(json[p]);
	}

	return sb.toString();
};
/**
 * 将form表单的的参数转化为json
 * @param parms
 * @returns {___anonymous10175_10176}
 */
JSON.parmsToJSON = function(form){

	var paramsArray = form.formToArray();
	var len = paramsArray.length;
	var json = {}, item = null;
	for(var i = 0; i< len; i++) {
		item = paramsArray[i];
		json[item.name] = item.value;
	}

	return json;
};

/*******************************************************************************
 * @Class Date
 ******************************************************************************/
var dateFormat = function() {
	var token = /d{1,4}|m{1,4}|yy(?:yy)?|([HhMsTt])\1?|[LloSZ]|"[^"]*"|'[^']*'/g, timezone = /\b(?:[PMCEA][SDP]T|(?:Pacific|Mountain|Central|Eastern|Atlantic) (?:Standard|Daylight|Prevailing) Time|(?:GMT|UTC)(?:[-+]\d{4})?)\b/g, timezoneClip = /[^-+\dA-Z]/g, pad = function(
			val, len) {
		val = String(val);
		len = len || 2;
		while (val.length < len)
			val = "0" + val;
		return val;
	};

	// Regexes and supporting functions are cached through closure
	return function(date, mask, utc) {
		var dF = dateFormat;

		// You can't provide utc if you skip other args (use the "UTC:" mask
		// prefix)
		if (arguments.length == 1 && Object.prototype.toString.call(date) == "[object String]" && !/\d/.test(date)) {
			mask = date;
			date = undefined;
		}

		// Passing date through Date applies Date.parse, if necessary
		date = date ? new Date(date) : new Date;
		if (isNaN(date))
			throw SyntaxError("invalid date");

		mask = String(dF.masks[mask] || mask || dF.masks["default"]);

		// Allow setting the utc argument via the mask
		if (mask.slice(0, 4) == "UTC:") {
			mask = mask.slice(4);
			utc = true;
		}

		var _ = utc ? "getUTC" : "get", d = date[_ + "Date"](), D = date[_ + "Day"](), m = date[_ + "Month"](), y = date[_
				+ "FullYear"](), H = date[_ + "Hours"](), M = date[_ + "Minutes"](), s = date[_ + "Seconds"](), L = date[_
				+ "Milliseconds"](), o = utc ? 0 : date.getTimezoneOffset(), flags = {
			d : d,
			dd : pad(d),
			ddd : dF.i18n.dayNames[D],
			dddd : dF.i18n.dayNames[D + 7],
			m : m + 1,
			mm : pad(m + 1),
			mmm : dF.i18n.monthNames[m],
			mmmm : dF.i18n.monthNames[m + 12],
			yy : String(y).slice(2),
			yyyy : y,
			h : H % 12 || 12,
			hh : pad(H % 12 || 12),
			H : H,
			HH : pad(H),
			M : M,
			MM : pad(M),
			s : s,
			ss : pad(s),
			l : pad(L, 3),
			L : pad(L > 99 ? Math.round(L / 10) : L),
			t : H < 12 ? "a" : "p",
			tt : H < 12 ? "am" : "pm",
			T : H < 12 ? "A" : "P",
			TT : H < 12 ? "AM" : "PM",
			Z : utc ? "UTC" : (String(date).match(timezone) || [ "" ]).pop().replace(timezoneClip, ""),
			o : (o > 0 ? "-" : "+") + pad(Math.floor(Math.abs(o) / 60) * 100 + Math.abs(o) % 60, 4),
			S : [ "th", "st", "nd", "rd" ][d % 10 > 3 ? 0 : (d % 100 - d % 10 != 10) * d % 10]
		};

		return mask.replace(token, function($0) {
			return $0 in flags ? flags[$0] : $0.slice(1, $0.length - 1);
		});
	};
}();

// Some common format strings
dateFormat.masks = {
	"default" : "ddd mmm dd yyyy HH:MM:ss",
	shortDate : "m/d/yy",
	mediumDate : "mmm d, yyyy",
	longDate : "mmmm d, yyyy",
	fullDate : "dddd, mmmm d, yyyy",
	shortTime : "h:MM TT",
	mediumTime : "h:MM:ss TT",
	longTime : "h:MM:ss TT Z",
	isoDate : "yyyy-mm-dd",
	isoTime : "HH:MM:ss",
	isoDateTime : "yyyy-mm-dd'T'HH:MM:ss",
	isoUtcDateTime : "UTC:yyyy-mm-dd'T'HH:MM:ss'Z'"
};

// Internationalization strings
dateFormat.i18n = {
	dayNames : [ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sunday", "Monday", "Tuesday", "Wednesday",
			"Thursday", "Friday", "Saturday" ],
	monthNames : [ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "January",
			"February", "March", "April", "May", "June", "July", "August", "September", "October", "November",
			"December" ]
};

// For convenience...
Date.prototype.format = function(mask, utc) {
	return dateFormat(this, mask, utc);
};

(function($) {
	$.fn.disable = function() {
		return this.each(function() {
			$(this).attr("disabled", true);
			// if (typeof this.disabled != "undefined") this.disabled = true;
		});
	};

	$.fn.enable = function() {
		return this.each(function() {
			$(this).attr("disabled", false);
			// if (typeof this.disabled != "undefined") this.disabled = false;
		});
	};
	
	$.fn.pannel = function(options) {
		var opts = $.extend({}, $.fn.pannel.defaults, options);
		var sb = new StringBuilder();
		sb.append('<div class="tab_menu">');
		sb.append('<ul class="tabs">');
		
		var items = opts.items;
		for (var i = 0, max = items.length; i < max; i++) {
			sb.append(String.format('<li><a href="javascript:;">{0}</a></li>', items[i].title));
		}
		
		sb.append('</ul>');
		sb.append('</div>');
		sb.append('<div class="tab_menu_bottom"></div>');
		sb.append('<div id="tabbody"></div>');
		
		$(this).html(sb.toString());

	    var $lis = $("ul.tabs a"), $body = $('#tabbody');
	    var currentIndex = -1;
	    $lis.bind('click', function() {
			var index = $lis.index(this);
			if (index == currentIndex) {
				return false;
			}
			
			currentIndex = index;
			$lis.removeClass("current");
			$(this).addClass("current");
			
			opts.tabChanging(index);
			
			var url = items[index].url;
			if (url == null) {
				$body.hide();
			} else {
				// Page.getHtml(url, items[index].cmd, function(data) {
				// 	opts.tabChanged(index);
				// 	$body.html(data).show();
				// });
                opts.tabChanged(index);
                $body.load(url);
			}
	    });
	    
	    opts.init($lis);
	    
	    return $(this);
	};
	
	$.fn.pannel.defaults = {
		items: [{ title: '', url: '' }],
		init: function() {},
		tabChanging: function(index) {},
		tabChanged: function(index) {}
	};
	
	$.fn.scrollNavInit = function($up, $down, step) {
		var $this = $(this);
		var h = $this.prop("scrollHeight");
		var top = parseInt($this.css("top"));
		if (top >= 0) {
			$up.hide();
		} else {
			$up.show();
		}

		if ((top * (-1) + step) >= h - 3) {
			$down.hide();
		} else {
			$down.show();
		}
	};

	$.fn.scrollUp = function(step) {
		var $this = $(this);
		var top = parseInt($this.css("top"));
		if (top < 0) {
			top = Math.min(top + step, 0);
			$this.css("top", top + "px");
		}
	};

	$.fn.scrollDown = function(step) {
		var $this = $(this);
		var h = $this.prop("scrollHeight");
		var top = -1 * parseInt($this.css("top"));
		if (top <= h - step) {
			top = Math.min(top + step, h);
			$this.css("top", -top + "px");
		}
	};
})(jQuery);

(function($){
	$.fn.inputDefault = function(options){
		var defaults = {attrName: 'emptytext', size:0, bold: false, italic:false, color:'#CCC'};
		var options = $.extend(defaults, options);
		this.each(function(){
			var $this = $(this);
			var text = $this.attr(options.attrName);
			
			var offset = $this.position();
	
			var outerWidth = $this.outerWidth();
			var outerHeight = $this.outerHeight();
			
			var innerWidth = $this.innerWidth();
			var innerHeight = $this.innerHeight();
			
			var plusLeft = (outerWidth - innerWidth)/2;
			var plusTop = (outerHeight - innerHeight)/2;
			
			var paddingTop = parseInt($this.css('paddingTop'));
			var paddingRight = parseInt($this.css('paddingRight'));
			var paddingBottom = parseInt($this.css('paddingBottom'));
			var paddingLeft = parseInt($this.css('paddingLeft'));
			
			if(!$.browser.chrome){
				var width = innerWidth - (paddingLeft + paddingRight);
				var height = innerHeight - (paddingTop + paddingBottom);
			}else{
				var width = innerWidth - paddingRight;
				var height = innerHeight - paddingBottom;
			}
		
			var top = offset.top + plusTop;
			var left = offset.left + plusLeft;
			
			var lineHeight = $this.css('lineHeight');
			var display = $this.val() ? 'none' : 'block';
			
			var fontSize = options.size ? options.size : $this.css('fontSize');
			var fontStyle = options.italic ? 'italic' : '';
			var fontWeight = options.bold ? '700' : $this.css('fontWeight');
			
			var css = {position:'absolute', fontSize: fontSize, fontWeight:fontWeight, fontStyle:fontStyle, lineHeight:lineHeight, display:display, paddingTop:paddingTop, paddingRight:paddingRight, paddingBottom:paddingBottom, paddingLeft:paddingLeft, cursor:'text', width:width, height:height, top:top, left:left, color:options.color, overflow:'hidden'};
			
			var lable = $("<label></label>").text(text).css(css).click(function(){
				$(this).hide();
				$(this).prev().focus();
			});
			
			$this.after(lable);
		}).focus(function(){
			var $this = $(this);
			var $label = $(this).next('label');
			$label.hide();
		}).blur(function(){
			var $this = $(this);
			var $label = $(this).next('label');
			if(!$this.val()) $label.show();
		});
	}
})(jQuery);

function GB2312UTF8() {
	this.Dig2Dec = function(s) {
		var retV = 0;
		if (s.length == 4) {
			for ( var i = 0; i < 4; i++) {
				retV += eval(s.charAt(i)) * Math.pow(2, 3 - i);
			}
			return retV;
		}
		return -1;
	};
	this.Hex2Utf8 = function(s) {
		var retS = "";
		var tempS = "";
		var ss = "";
		if (s.length == 16) {
			tempS = "1110" + s.substring(0, 4);
			tempS += "10" + s.substring(4, 10);
			tempS += "10" + s.substring(10, 16);
			var sss = "0123456789ABCDEF";
			for ( var i = 0; i < 3; i++) {
				retS += "%";
				ss = tempS.substring(i * 8, (eval(i) + 1) * 8);
				retS += sss.charAt(this.Dig2Dec(ss.substring(0, 4)));
				retS += sss.charAt(this.Dig2Dec(ss.substring(4, 8)));
			}
			return retS;
		}
		return "";
	};
	this.Dec2Dig = function(n1) {
		var s = "";
		var n2 = 0;
		for ( var i = 0; i < 4; i++) {
			n2 = Math.pow(2, 3 - i);
			if (n1 >= n2) {
				s += '1';
				n1 = n1 - n2;
			} else
				s += '0';
		}
		return s;
	};

	this.Str2Hex = function(s) {
		var c = "";
		var n;
		var ss = "0123456789ABCDEF";
		var digS = "";
		for ( var i = 0; i < s.length; i++) {
			c = s.charAt(i);
			n = ss.indexOf(c);
			digS += this.Dec2Dig(eval(n));
		}
		return digS;
	};
	this.convertGB2312ToUTF8 = function(s1) {
		var s = escape(s1);
		var sa = s.split("%");
		var retV = "";
		if (sa[0] != "") {
			retV = sa[0];
		}
		for ( var i = 1; i < sa.length; i++) {
			if (sa[i].substring(0, 1) == "u") {
				retV += this.Hex2Utf8(this.Str2Hex(sa[i].substring(1, 5)));
				if (sa[i].length) {
					retV += sa[i].substring(5);
				}
			} else {
				retV += ("%" + sa[i]);
				if (sa[i].length) {
					retV += sa[i].substring(5);
				}
			}
		}
		return retV;
	};
	this.convertUTF8ToGB2312 = function(str1) {
		var substr = "";
		var a = "";
		var b = "";
		var c = "";
		var i = -1;
		i = str1.indexOf("%");
		if (i == -1) {
			return str1;
		}
		while (i != -1) {
			if (i < 3) {
				substr = substr + str1.substr(0, i - 1);
				str1 = str1.substr(i + 1, str1.length - i);
				a = str1.substr(0, 2);
				str1 = str1.substr(2, str1.length - 2);
				if (parseInt("0x" + a) & 0x80 == 0) {
					substr = substr + String.fromCharCode(parseInt("0x" + a));
				} else if (parseInt("0x" + a) & 0xE0 == 0xC0) { // two byte
					b = str1.substr(1, 2);
					str1 = str1.substr(3, str1.length - 3);
					var widechar = (parseInt("0x" + a) & 0x1F) << 6;
					widechar = widechar | (parseInt("0x" + b) & 0x3F);
					substr = substr + String.fromCharCode(widechar);
				} else {
					b = str1.substr(1, 2);
					str1 = str1.substr(3, str1.length - 3);
					c = str1.substr(1, 2);
					str1 = str1.substr(3, str1.length - 3);
					var widechar = (parseInt("0x" + a) & 0x0F) << 12;
					widechar = widechar | ((parseInt("0x" + b) & 0x3F) << 6);
					widechar = widechar | (parseInt("0x" + c) & 0x3F);
					substr = substr + String.fromCharCode(widechar);
				}
			} else {
				substr = substr + str1.substring(0, i);
				str1 = str1.substring(i);
			}
			i = str1.indexOf("%");
		}

		return substr + str1;
	};
}

function Timezone(value, en, cn) {
	this.en = en;
	this.cn = cn;
	this.value = value;
}

Timezone.getAllZones = function() {
	var zones = [];
	zones.push(new Timezone('GMT0@Africa/Abidjan', 'Africa/Abidjan', ''));
	zones.push(new Timezone('GMT0@Africa/Accra', 'Africa/Accra', ''));
	zones.push(new Timezone('EAT-3@Africa/Addis_Ababa', 'Africa/Addis Ababa', ''));
	zones.push(new Timezone('CET-1@Africa/Algiers', 'Africa/Algiers', ''));
	zones.push(new Timezone('EAT-3@Africa/Asmara', 'Africa/Asmara', ''));
	zones.push(new Timezone('GMT0@Africa/Bamako', 'Africa/Bamako', ''));
	zones.push(new Timezone('WAT-1@Africa/Bangui', 'Africa/Bangui', ''));
	zones.push(new Timezone('GMT0@Africa/Banjul', 'Africa/Banjul', ''));
	zones.push(new Timezone('GMT0@Africa/Bissau', 'Africa/Bissau', ''));
	zones.push(new Timezone('CAT-2@Africa/Blantyre', 'Africa/Blantyre', ''));
	zones.push(new Timezone('WAT-1@Africa/Brazzaville', 'Africa/Brazzaville', ''));
	zones.push(new Timezone('CAT-2@Africa/Bujumbura', 'Africa/Bujumbura', ''));
	zones.push(new Timezone('WET0@Africa/Casablanca', 'Africa/Casablanca', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Africa/Ceuta', 'Africa/Ceuta', ''));
	zones.push(new Timezone('GMT0@Africa/Conakry', 'Africa/Conakry', ''));
	zones.push(new Timezone('GMT0@Africa/Dakar', 'Africa/Dakar', ''));
	zones.push(new Timezone('EAT-3@Africa/Dar_es_Salaam', 'Africa/Dar es Salaam', ''));
	zones.push(new Timezone('EAT-3@Africa/Djibouti', 'Africa/Djibouti', ''));
	zones.push(new Timezone('WAT-1@Africa/Douala', 'Africa/Douala', ''));
	zones.push(new Timezone('WET0@Africa/El_Aaiun', 'Africa/El Aaiun', ''));
	zones.push(new Timezone('GMT0@Africa/Freetown', 'Africa/Freetown', ''));
	zones.push(new Timezone('CAT-2@Africa/Gaborone', 'Africa/Gaborone', ''));
	zones.push(new Timezone('CAT-2@Africa/Harare', 'Africa/Harare', ''));
	zones.push(new Timezone('SAST-2@Africa/Johannesburg', 'Africa/Johannesburg', ''));
	zones.push(new Timezone('EAT-3@Africa/Kampala', 'Africa/Kampala', ''));
	zones.push(new Timezone('EAT-3@Africa/Khartoum', 'Africa/Khartoum', ''));
	zones.push(new Timezone('CAT-2@Africa/Kigali', 'Africa/Kigali', ''));
	zones.push(new Timezone('WAT-1@Africa/Kinshasa', 'Africa/Kinshasa', ''));
	zones.push(new Timezone('WAT-1@Africa/Lagos', 'Africa/Lagos', ''));
	zones.push(new Timezone('WAT-1@Africa/Libreville', 'Africa/Libreville', ''));
	zones.push(new Timezone('GMT0@Africa/Lome', 'Africa/Lome', ''));
	zones.push(new Timezone('WAT-1@Africa/Luanda', 'Africa/Luanda', ''));
	zones.push(new Timezone('CAT-2@Africa/Lubumbashi', 'Africa/Lubumbashi', ''));
	zones.push(new Timezone('CAT-2@Africa/Lusaka', 'Africa/Lusaka', ''));
	zones.push(new Timezone('WAT-1@Africa/Malabo', 'Africa/Malabo', ''));
	zones.push(new Timezone('CAT-2@Africa/Maputo', 'Africa/Maputo', ''));
	zones.push(new Timezone('SAST-2@Africa/Maseru', 'Africa/Maseru', ''));
	zones.push(new Timezone('SAST-2@Africa/Mbabane', 'Africa/Mbabane', ''));
	zones.push(new Timezone('EAT-3@Africa/Mogadishu', 'Africa/Mogadishu', ''));
	zones.push(new Timezone('GMT0@Africa/Monrovia', 'Africa/Monrovia', ''));
	zones.push(new Timezone('EAT-3@Africa/Nairobi', 'Africa/Nairobi', ''));
	zones.push(new Timezone('WAT-1@Africa/Ndjamena', 'Africa/Ndjamena', ''));
	zones.push(new Timezone('WAT-1@Africa/Niamey', 'Africa/Niamey', ''));
	zones.push(new Timezone('GMT0@Africa/Nouakchott', 'Africa/Nouakchott', ''));
	zones.push(new Timezone('GMT0@Africa/Ouagadougou', 'Africa/Ouagadougou', ''));
	zones.push(new Timezone('WAT-1@Africa/Porto-Novo', 'Africa/Porto-Novo', ''));
	zones.push(new Timezone('GMT0@Africa/Sao_Tome', 'Africa/Sao Tome', ''));
	zones.push(new Timezone('EET-2@Africa/Tripoli', 'Africa/Tripoli', ''));
	zones.push(new Timezone('CET-1@Africa/Tunis', 'Africa/Tunis', ''));
	zones.push(new Timezone('WAT-1WAST,M9.1.0,M4.1.0@Africa/Windhoek', 'Africa/Windhoek', ''));
	zones.push(new Timezone('HAST10HADT,M3.2.0,M11.1.0@America/Adak', 'America/Adak', ''));
	zones.push(new Timezone('AKST9AKDT,M3.2.0,M11.1.0@America/Anchorage', 'America/Anchorage', ''));
	zones.push(new Timezone('AST4@America/Anguilla', 'America/Anguilla', ''));
	zones.push(new Timezone('AST4@America/Antigua', 'America/Antigua', ''));
	zones.push(new Timezone('BRT3@America/Araguaina', 'America/Araguaina', ''));
	zones.push(new Timezone('ART3@America/Buenos_Aires', 'America/Argentina/Buenos Aires', ''));
	zones.push(new Timezone('ART3@America/Catamarca', 'America/Argentina/Catamarca', ''));
	zones.push(new Timezone('ART3@America/Cordoba', 'America/Argentina/Cordoba', ''));
	zones.push(new Timezone('ART3@America/Jujuy', 'America/Argentina/Jujuy', ''));
	zones.push(new Timezone('ART3@America/Argentina/La_Rioja', 'America/Argentina/La Rioja', ''));
	zones.push(new Timezone('ART3@America/Mendoza', 'America/Argentina/Mendoza', ''));
	zones.push(new Timezone('ART3@America/Argentina/Rio_Gallegos', 'America/Argentina/Rio Gallegos', ''));
	zones.push(new Timezone('ART3@America/Argentina/Salta', 'America/Argentina/Salta', ''));
	zones.push(new Timezone('ART3@America/Argentina/San_Juan', 'America/Argentina/San Juan', ''));
	zones.push(new Timezone('ART3@America/Argentina/Tucuman', 'America/Argentina/Tucuman', ''));
	zones.push(new Timezone('ART3@America/Argentina/Ushuaia', 'America/Argentina/Ushuaia', ''));
	zones.push(new Timezone('AST4@America/Aruba', 'America/Aruba', ''));
	zones.push(new Timezone('PYT4PYST,M10.1.0/0,M4.2.0/0@America/Asuncion', 'America/Asuncion', ''));
	zones.push(new Timezone('EST5@America/Atikokan', 'America/Atikokan', ''));
	zones.push(new Timezone('BRT3@America/Bahia', 'America/Bahia', ''));
	zones.push(new Timezone('AST4@America/Barbados', 'America/Barbados', ''));
	zones.push(new Timezone('BRT3@America/Belem', 'America/Belem', ''));
	zones.push(new Timezone('CST6@America/Belize', 'America/Belize', ''));
	zones.push(new Timezone('AST4@America/Blanc-Sablon', 'America/Blanc-Sablon', ''));
	zones.push(new Timezone('AMT4@America/Boa_Vista', 'America/Boa Vista', ''));
	zones.push(new Timezone('COT5@America/Bogota', 'America/Bogota', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Boise', 'America/Boise', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Cambridge_Bay', 'America/Cambridge Bay', ''));
	zones.push(new Timezone('AMT4AMST,M10.3.0/0,M2.3.0/0@America/Campo_Grande', 'America/Campo Grande', ''));
	zones.push(new Timezone('CST6CDT,M4.1.0,M10.5.0@America/Cancun', 'America/Cancun', ''));
	zones.push(new Timezone('VET4:30@America/Caracas', 'America/Caracas', ''));
	zones.push(new Timezone('GFT3@America/Cayenne', 'America/Cayenne', ''));
	zones.push(new Timezone('EST5@America/Cayman', 'America/Cayman', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Chicago', 'America/Chicago', ''));
	zones.push(new Timezone('MST7MDT,M4.1.0,M10.5.0@America/Chihuahua', 'America/Chihuahua', ''));
	zones.push(new Timezone('CST6@America/Costa_Rica', 'America/Costa Rica', ''));
	zones.push(new Timezone('AMT4AMST,M10.3.0/0,M2.3.0/0@America/Cuiaba', 'America/Cuiaba', ''));
	zones.push(new Timezone('AST4@America/Curacao', 'America/Curacao', ''));
	zones.push(new Timezone('GMT0@America/Danmarkshavn', 'America/Danmarkshavn', ''));
	zones.push(new Timezone('PST8PDT,M3.2.0,M11.1.0@America/Dawson', 'America/Dawson', ''));
	zones.push(new Timezone('MST7@America/Dawson_Creek', 'America/Dawson Creek', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Denver', 'America/Denver', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Detroit', 'America/Detroit', ''));
	zones.push(new Timezone('AST4@America/Dominica', 'America/Dominica', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Edmonton', 'America/Edmonton', ''));
	zones.push(new Timezone('AMT4@America/Eirunepe', 'America/Eirunepe', ''));
	zones.push(new Timezone('CST6@America/El_Salvador', 'America/El Salvador', ''));
	zones.push(new Timezone('BRT3@America/Fortaleza', 'America/Fortaleza', ''));
	zones.push(new Timezone('AST4ADT,M3.2.0,M11.1.0@America/Glace_Bay', 'America/Glace Bay', ''));
	zones.push(new Timezone('AST4ADT,M3.2.0/0:01,M11.1.0/0:01@America/Goose_Bay', 'America/Goose Bay', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Grand_Turk', 'America/Grand Turk', ''));
	zones.push(new Timezone('AST4@America/Grenada', 'America/Grenada', ''));
	zones.push(new Timezone('AST4@America/Guadeloupe', 'America/Guadeloupe', ''));
	zones.push(new Timezone('CST6@America/Guatemala', 'America/Guatemala', ''));
	zones.push(new Timezone('ECT5@America/Guayaquil', 'America/Guayaquil', ''));
	zones.push(new Timezone('GYT4@America/Guyana', 'America/Guyana', ''));
	zones.push(new Timezone('AST4ADT,M3.2.0,M11.1.0@America/Halifax', 'America/Halifax', ''));
	zones.push(new Timezone('CST5CDT,M3.2.0/0,M10.5.0/1@America/Havana', 'America/Havana', ''));
	zones.push(new Timezone('MST7@America/Hermosillo', 'America/Hermosillo', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Indiana/Indianapolis', 'America/Indiana/Indianapolis', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Indiana/Knox', 'America/Indiana/Knox', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Indiana/Marengo', 'America/Indiana/Marengo', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Indiana/Petersburg', 'America/Indiana/Petersburg', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Indiana/Tell_City', 'America/Indiana/Tell City', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Indiana/Vevay', 'America/Indiana/Vevay', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Indiana/Vincennes', 'America/Indiana/Vincennes', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Indiana/Winamac', 'America/Indiana/Winamac', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Inuvik', 'America/Inuvik', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Iqaluit', 'America/Iqaluit', ''));
	zones.push(new Timezone('EST5@America/Jamaica', 'America/Jamaica', ''));
	zones.push(new Timezone('AKST9AKDT,M3.2.0,M11.1.0@America/Juneau', 'America/Juneau', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Kentucky/Louisville', 'America/Kentucky/Louisville', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Kentucky/Monticello', 'America/Kentucky/Monticello', ''));
	zones.push(new Timezone('BOT4@America/La_Paz', 'America/La Paz', ''));
	zones.push(new Timezone('PET5@America/Lima', 'America/Lima', ''));
	zones.push(new Timezone('PST8PDT,M3.2.0,M11.1.0@America/Los_Angeles', 'America/Los Angeles', ''));
	zones.push(new Timezone('BRT3@America/Maceio', 'America/Maceio', ''));
	zones.push(new Timezone('CST6@America/Managua', 'America/Managua', ''));
	zones.push(new Timezone('AMT4@America/Manaus', 'America/Manaus', ''));
	zones.push(new Timezone('AST4@America/Marigot', 'America/Marigot', ''));
	zones.push(new Timezone('AST4@America/Martinique', 'America/Martinique', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Matamoros', 'America/Matamoros', ''));
	zones.push(new Timezone('MST7MDT,M4.1.0,M10.5.0@America/Mazatlan', 'America/Mazatlan', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Menominee', 'America/Menominee', ''));
	zones.push(new Timezone('CST6CDT,M4.1.0,M10.5.0@America/Merida', 'America/Merida', ''));
	zones.push(new Timezone('CST6CDT,M4.1.0,M10.5.0@America/Mexico_City', 'America/Mexico City', ''));
	zones.push(new Timezone('PMST3PMDT,M3.2.0,M11.1.0@America/Miquelon', 'America/Miquelon', ''));
	zones.push(new Timezone('AST4ADT,M3.2.0,M11.1.0@America/Moncton', 'America/Moncton', ''));
	zones.push(new Timezone('CST6CDT,M4.1.0,M10.5.0@America/Monterrey', 'America/Monterrey', ''));
	zones.push(new Timezone('UYT3UYST,M10.1.0,M3.2.0@America/Montevideo', 'America/Montevideo', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Montreal', 'America/Montreal', ''));
	zones.push(new Timezone('AST4@America/Montserrat', 'America/Montserrat', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Nassau', 'America/Nassau', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/New_York', 'America/New York', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Nipigon', 'America/Nipigon', ''));
	zones.push(new Timezone('AKST9AKDT,M3.2.0,M11.1.0@America/Nome', 'America/Nome', ''));
	zones.push(new Timezone('FNT2@America/Noronha', 'America/Noronha', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/North_Dakota/Center', 'America/North Dakota/Center', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/North_Dakota/New_Salem', 'America/North Dakota/New Salem', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Ojinaga', 'America/Ojinaga', ''));
	zones.push(new Timezone('EST5@America/Panama', 'America/Panama', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Pangnirtung', 'America/Pangnirtung', ''));
	zones.push(new Timezone('SRT3@America/Paramaribo', 'America/Paramaribo', ''));
	zones.push(new Timezone('MST7@America/Phoenix', 'America/Phoenix', ''));
	zones.push(new Timezone('AST4@America/Port_of_Spain', 'America/Port of Spain', ''));
	zones.push(new Timezone('EST5@America/Port-au-Prince', 'America/Port-au-Prince', ''));
	zones.push(new Timezone('AMT4@America/Porto_Velho', 'America/Porto Velho', ''));
	zones.push(new Timezone('AST4@America/Puerto_Rico', 'America/Puerto Rico', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Rainy_River', 'America/Rainy River', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Rankin_Inlet', 'America/Rankin Inlet', ''));
	zones.push(new Timezone('BRT3@America/Recife', 'America/Recife', ''));
	zones.push(new Timezone('CST6@America/Regina', 'America/Regina', ''));
	zones.push(new Timezone('AMT4@America/Rio_Branco', 'America/Rio Branco', ''));
	zones.push(new Timezone('PST8PDT,M4.1.0,M10.5.0@America/Santa_Isabel', 'America/Santa Isabel', ''));
	zones.push(new Timezone('BRT3@America/Santarem', 'America/Santarem', ''));
	zones.push(new Timezone('AST4@America/Santo_Domingo', 'America/Santo Domingo', ''));
	zones.push(new Timezone('BRT3BRST,M10.3.0/0,M2.3.0/0@America/Sao_Paulo', 'America/Sao Paulo', ''));
	zones.push(new Timezone('EGT1EGST,M3.5.0/0,M10.5.0/1@America/Scoresbysund', 'America/Scoresbysund', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Shiprock', 'America/Shiprock', ''));
	zones.push(new Timezone('AST4@America/St_Barthelemy', 'America/St Barthelemy', ''));
	zones.push(new Timezone('NST3:30NDT,M3.2.0/0:01,M11.1.0/0:01@America/St_Johns', 'America/St Johns', ''));
	zones.push(new Timezone('AST4@America/St_Kitts', 'America/St Kitts', ''));
	zones.push(new Timezone('AST4@America/St_Lucia', 'America/St Lucia', ''));
	zones.push(new Timezone('AST4@America/St_Thomas', 'America/St Thomas', ''));
	zones.push(new Timezone('AST4@America/St_Vincent', 'America/St Vincent', ''));
	zones.push(new Timezone('CST6@America/Swift_Current', 'America/Swift Current', ''));
	zones.push(new Timezone('CST6@America/Tegucigalpa', 'America/Tegucigalpa', ''));
	zones.push(new Timezone('AST4ADT,M3.2.0,M11.1.0@America/Thule', 'America/Thule', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Thunder_Bay', 'America/Thunder Bay', ''));
	zones.push(new Timezone('PST8PDT,M3.2.0,M11.1.0@America/Tijuana', 'America/Tijuana', ''));
	zones.push(new Timezone('EST5EDT,M3.2.0,M11.1.0@America/Toronto', 'America/Toronto', ''));
	zones.push(new Timezone('AST4@America/Tortola', 'America/Tortola', ''));
	zones.push(new Timezone('PST8PDT,M3.2.0,M11.1.0@America/Vancouver', 'America/Vancouver', ''));
	zones.push(new Timezone('PST8PDT,M3.2.0,M11.1.0@America/Whitehorse', 'America/Whitehorse', ''));
	zones.push(new Timezone('CST6CDT,M3.2.0,M11.1.0@America/Winnipeg', 'America/Winnipeg', ''));
	zones.push(new Timezone('AKST9AKDT,M3.2.0,M11.1.0@America/Yakutat', 'America/Yakutat', ''));
	zones.push(new Timezone('MST7MDT,M3.2.0,M11.1.0@America/Yellowknife', 'America/Yellowknife', ''));
	zones.push(new Timezone('WST-8@Antarctica/Casey', 'Antarctica/Casey', ''));
	zones.push(new Timezone('DAVT-7@Antarctica/Davis', 'Antarctica/Davis', ''));
	zones.push(new Timezone('DDUT-10@Antarctica/DumontDUrville', 'Antarctica/DumontDUrville', ''));
	zones.push(new Timezone('MIST-11@Antarctica/Macquarie', 'Antarctica/Macquarie', ''));
	zones.push(new Timezone('MAWT-5@Antarctica/Mawson', 'Antarctica/Mawson', ''));
	zones.push(new Timezone('NZST-12NZDT,M9.5.0,M4.1.0/3@Antarctica/McMurdo', 'Antarctica/McMurdo', ''));
	zones.push(new Timezone('ROTT3@Antarctica/Rothera', 'Antarctica/Rothera', ''));
	zones.push(new Timezone('NZST-12NZDT,M9.5.0,M4.1.0/3@Antarctica/South_Pole', 'Antarctica/South Pole', ''));
	zones.push(new Timezone('SYOT-3@Antarctica/Syowa', 'Antarctica/Syowa', ''));
	zones.push(new Timezone('VOST-6@Antarctica/Vostok', 'Antarctica/Vostok', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Arctic/Longyearbyen', 'Arctic/Longyearbyen', ''));
	zones.push(new Timezone('AST-3@Asia/Aden', 'Asia/Aden', ''));
	zones.push(new Timezone('ALMT-6@Asia/Almaty', 'Asia/Almaty', ''));
	zones.push(new Timezone('ANAT-11ANAST,M3.5.0,M10.5.0/3@Asia/Anadyr', 'Asia/Anadyr', ''));
	zones.push(new Timezone('AQTT-5@Asia/Aqtau', 'Asia/Aqtau', ''));
	zones.push(new Timezone('AQTT-5@Asia/Aqtobe', 'Asia/Aqtobe', ''));
	zones.push(new Timezone('TMT-5@Asia/Ashgabat', 'Asia/Ashgabat', ''));
	zones.push(new Timezone('AST-3@Asia/Baghdad', 'Asia/Baghdad', ''));
	zones.push(new Timezone('AST-3@Asia/Bahrain', 'Asia/Bahrain', ''));
	zones.push(new Timezone('AZT-4AZST,M3.5.0/4,M10.5.0/5@Asia/Baku', 'Asia/Baku', ''));
	zones.push(new Timezone('ICT-7@Asia/Bangkok', 'Asia/Bangkok', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/0,M10.5.0/0@Asia/Beirut', 'Asia/Beirut', ''));
	zones.push(new Timezone('KGT-6@Asia/Bishkek', 'Asia/Bishkek', ''));
	zones.push(new Timezone('BNT-8@Asia/Brunei', 'Asia/Brunei', ''));
	zones.push(new Timezone('CHOT-8@Asia/Choibalsan', 'Asia/Choibalsan', ''));
	zones.push(new Timezone('CST-8@Asia/Chongqing', 'Asia/Chongqing', ''));
	zones.push(new Timezone('IST-5:30@Asia/Colombo', 'Asia/Colombo', ''));
	zones.push(new Timezone('EET-2EEST,M4.1.5/0,M10.5.5/0@Asia/Damascus', 'Asia/Damascus', ''));
	zones.push(new Timezone('BDT-6@Asia/Dhaka', 'Asia/Dhaka', ''));
	zones.push(new Timezone('TLT-9@Asia/Dili', 'Asia/Dili', ''));
	zones.push(new Timezone('GST-4@Asia/Dubai', 'Asia/Dubai', ''));
	zones.push(new Timezone('TJT-5@Asia/Dushanbe', 'Asia/Dushanbe', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.6/0:01,M9.1.5@Asia/Gaza', 'Asia/Gaza', ''));
	zones.push(new Timezone('CST-8@Asia/Harbin', 'Asia/Harbin', ''));
	zones.push(new Timezone('ICT-7@Asia/Ho_Chi_Minh', 'Asia/Ho Chi Minh', ''));
	zones.push(new Timezone('HKT-8@Asia/Hong_Kong', 'Asia/Hong Kong', ''));
	zones.push(new Timezone('HOVT-7@Asia/Hovd', 'Asia/Hovd', ''));
	zones.push(new Timezone('IRKT-8IRKST,M3.5.0,M10.5.0/3@Asia/Irkutsk', 'Asia/Irkutsk', ''));
	zones.push(new Timezone('WIT-7@Asia/Jakarta', 'Asia/Jakarta', ''));
	zones.push(new Timezone('EIT-9@Asia/Jayapura', 'Asia/Jayapura', ''));
	zones.push(new Timezone('AFT-4:30@Asia/Kabul', 'Asia/Kabul', ''));
	zones.push(new Timezone('PETT-11PETST,M3.5.0,M10.5.0/3@Asia/Kamchatka', 'Asia/Kamchatka', ''));
	zones.push(new Timezone('PKT-5@Asia/Karachi', 'Asia/Karachi', ''));
	zones.push(new Timezone('CST-8@Asia/Kashgar', 'Asia/Kashgar', ''));
	zones.push(new Timezone('NPT-5:45@Asia/Kathmandu', 'Asia/Kathmandu', ''));
	zones.push(new Timezone('IST-5:30@Asia/Kolkata', 'Asia/Kolkata', ''));
	zones.push(new Timezone('KRAT-7KRAST,M3.5.0,M10.5.0/3@Asia/Krasnoyarsk', 'Asia/Krasnoyarsk', ''));
	zones.push(new Timezone('MYT-8@Asia/Kuala_Lumpur', 'Asia/Kuala Lumpur', ''));
	zones.push(new Timezone('MYT-8@Asia/Kuching', 'Asia/Kuching', ''));
	zones.push(new Timezone('AST-3@Asia/Kuwait', 'Asia/Kuwait', ''));
	zones.push(new Timezone('CST-8@Asia/Macau', 'Asia/Macau', ''));
	zones.push(new Timezone('MAGT-11MAGST,M3.5.0,M10.5.0/3@Asia/Magadan', 'Asia/Magadan', ''));
	zones.push(new Timezone('CIT-8@Asia/Makassar', 'Asia/Makassar', ''));
	zones.push(new Timezone('PHT-8@Asia/Manila', 'Asia/Manila', ''));
	zones.push(new Timezone('GST-4@Asia/Muscat', 'Asia/Muscat', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Asia/Nicosia', 'Asia/Nicosia', ''));
	zones.push(new Timezone('NOVT-6NOVST,M3.5.0,M10.5.0/3@Asia/Novokuznetsk', 'Asia/Novokuznetsk', ''));
	zones.push(new Timezone('NOVT-6NOVST,M3.5.0,M10.5.0/3@Asia/Novosibirsk', 'Asia/Novosibirsk', ''));
	zones.push(new Timezone('OMST-7@Asia/Omsk', 'Asia/Omsk', ''));
	zones.push(new Timezone('ORAT-5@Asia/Oral', 'Asia/Oral', ''));
	zones.push(new Timezone('ICT-7@Asia/Phnom_Penh', 'Asia/Phnom Penh', ''));
	zones.push(new Timezone('WIT-7@Asia/Pontianak', 'Asia/Pontianak', ''));
	zones.push(new Timezone('KST-9@Asia/Pyongyang', 'Asia/Pyongyang', ''));
	zones.push(new Timezone('AST-3@Asia/Qatar', 'Asia/Qatar', ''));
	zones.push(new Timezone('QYZT-6@Asia/Qyzylorda', 'Asia/Qyzylorda', ''));
	zones.push(new Timezone('MMT-6:30@Asia/Rangoon', 'Asia/Rangoon', ''));
	zones.push(new Timezone('AST-3@Asia/Riyadh', 'Asia/Riyadh', ''));
	zones.push(new Timezone('SAKT-10SAKST,M3.5.0,M10.5.0/3@Asia/Sakhalin', 'Asia/Sakhalin', ''));
	zones.push(new Timezone('UZT-5@Asia/Samarkand', 'Asia/Samarkand', ''));
	zones.push(new Timezone('KST-9@Asia/Seoul', 'Asia/Seoul', ''));
	zones.push(new Timezone('CST-8@Asia/Shanghai', 'Asia/Shanghai', ''));
	zones.push(new Timezone('SGT-8@Asia/Singapore', 'Asia/Singapore', ''));
	zones.push(new Timezone('CST-8@Asia/Taipei', 'Asia/Taipei', ''));
	zones.push(new Timezone('UZT-5@Asia/Tashkent', 'Asia/Tashkent', ''));
	zones.push(new Timezone('GET-4@Asia/Tbilisi', 'Asia/Tbilisi', ''));
	zones.push(new Timezone('IRST-3:30IRDT,80/0,264/0@Asia/Tehran', 'Asia/Tehran', ''));
	zones.push(new Timezone('BTT-6@Asia/Thimphu', 'Asia/Thimphu', ''));
	zones.push(new Timezone('JST-9@Asia/Tokyo', 'Asia/Tokyo', ''));
	zones.push(new Timezone('ULAT-8@Asia/Ulaanbaatar', 'Asia/Ulaanbaatar', ''));
	zones.push(new Timezone('CST-8@Asia/Urumqi', 'Asia/Urumqi', ''));
	zones.push(new Timezone('ICT-7@Asia/Vientiane', 'Asia/Vientiane', ''));
	zones.push(new Timezone('VLAT-10VLAST,M3.5.0,M10.5.0/3@Asia/Vladivostok', 'Asia/Vladivostok', ''));
	zones.push(new Timezone('YAKT-9YAKST,M3.5.0,M10.5.0/3@Asia/Yakutsk', 'Asia/Yakutsk', ''));
	zones.push(new Timezone('YEKT-5YEKST,M3.5.0,M10.5.0/3@Asia/Yekaterinburg', 'Asia/Yekaterinburg', ''));
	zones.push(new Timezone('AMT-4AMST,M3.5.0,M10.5.0/3@Asia/Yerevan', 'Asia/Yerevan', ''));
	zones.push(new Timezone('AZOT1AZOST,M3.5.0/0,M10.5.0/1@Atlantic/Azores', 'Atlantic/Azores', ''));
	zones.push(new Timezone('AST4ADT,M3.2.0,M11.1.0@Atlantic/Bermuda', 'Atlantic/Bermuda', ''));
	zones.push(new Timezone('WET0WEST,M3.5.0/1,M10.5.0@Atlantic/Canary', 'Atlantic/Canary', ''));
	zones.push(new Timezone('CVT1@Atlantic/Cape_Verde', 'Atlantic/Cape Verde', ''));
	zones.push(new Timezone('WET0WEST,M3.5.0/1,M10.5.0@Atlantic/Faroe', 'Atlantic/Faroe', ''));
	zones.push(new Timezone('WET0WEST,M3.5.0/1,M10.5.0@Atlantic/Madeira', 'Atlantic/Madeira', ''));
	zones.push(new Timezone('GMT0@Atlantic/Reykjavik', 'Atlantic/Reykjavik', ''));
	zones.push(new Timezone('GST2@Atlantic/South_Georgia', 'Atlantic/South Georgia', ''));
	zones.push(new Timezone('GMT0@Atlantic/St_Helena', 'Atlantic/St Helena', ''));
	zones.push(new Timezone('FKT4FKST,M9.1.0,M4.3.0@Atlantic/Stanley', 'Atlantic/Stanley', ''));
	zones.push(new Timezone('CST-9:30CST,M10.1.0,M4.1.0/3@Australia/Adelaide', 'Australia/Adelaide', ''));
	zones.push(new Timezone('EST-10@Australia/Brisbane', 'Australia/Brisbane', ''));
	zones.push(new Timezone('CST-9:30CST,M10.1.0,M4.1.0/3@Australia/Broken_Hill', 'Australia/Broken Hill', ''));
	zones.push(new Timezone('EST-10EST,M10.1.0,M4.1.0/3@Australia/Currie', 'Australia/Currie', ''));
	zones.push(new Timezone('CST-9:30@Australia/Darwin', 'Australia/Darwin', ''));
	zones.push(new Timezone('CWST-8:45@Australia/Eucla', 'Australia/Eucla', ''));
	zones.push(new Timezone('EST-10EST,M10.1.0,M4.1.0/3@Australia/Hobart', 'Australia/Hobart', ''));
	zones.push(new Timezone('EST-10@Australia/Lindeman', 'Australia/Lindeman', ''));
	zones.push(new Timezone('LHST-10:30LHST-11,M10.1.0,M4.1.0@Australia/Lord_Howe', 'Australia/Lord Howe', ''));
	zones.push(new Timezone('EST-10EST,M10.1.0,M4.1.0/3@Australia/Melbourne', 'Australia/Melbourne', ''));
	zones.push(new Timezone('WST-8@Australia/Perth', 'Australia/Perth', ''));
	zones.push(new Timezone('EST-10EST,M10.1.0,M4.1.0/3@Australia/Sydney', 'Australia/Sydney', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Amsterdam', 'Europe/Amsterdam', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Andorra', 'Europe/Andorra', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Athens', 'Europe/Athens', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Belgrade', 'Europe/Belgrade', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Berlin', 'Europe/Berlin', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Bratislava', 'Europe/Bratislava', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Brussels', 'Europe/Brussels', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Bucharest', 'Europe/Bucharest', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Budapest', 'Europe/Budapest', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Chisinau', 'Europe/Chisinau', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Copenhagen', 'Europe/Copenhagen', ''));
	zones.push(new Timezone('GMT0IST,M3.5.0/1,M10.5.0@Europe/Dublin', 'Europe/Dublin', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Gibraltar', 'Europe/Gibraltar', ''));
	zones.push(new Timezone('GMT0BST,M3.5.0/1,M10.5.0@Europe/Guernsey', 'Europe/Guernsey', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Helsinki', 'Europe/Helsinki', ''));
	zones.push(new Timezone('GMT0BST,M3.5.0/1,M10.5.0@Europe/Isle_of_Man', 'Europe/Isle of Man', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Istanbul', 'Europe/Istanbul', ''));
	zones.push(new Timezone('GMT0BST,M3.5.0/1,M10.5.0@Europe/Jersey', 'Europe/Jersey', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0,M10.5.0/3@Europe/Kaliningrad', 'Europe/Kaliningrad', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Kiev', 'Europe/Kiev', ''));
	zones.push(new Timezone('WET0WEST,M3.5.0/1,M10.5.0@Europe/Lisbon', 'Europe/Lisbon', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Ljubljana', 'Europe/Ljubljana', ''));
	zones.push(new Timezone('GMT0BST,M3.5.0/1,M10.5.0@Europe/London', 'Europe/London', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Luxembourg', 'Europe/Luxembourg', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Madrid', 'Europe/Madrid', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Malta', 'Europe/Malta', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Mariehamn', 'Europe/Mariehamn', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0,M10.5.0/3@Europe/Minsk', 'Europe/Minsk', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Monaco', 'Europe/Monaco', ''));
	zones.push(new Timezone('MSK-4@Europe/Moscow', 'Europe/Moscow', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Oslo', 'Europe/Oslo', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Paris', 'Europe/Paris', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Podgorica', 'Europe/Podgorica', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Prague', 'Europe/Prague', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Riga', 'Europe/Riga', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Rome', 'Europe/Rome', ''));
	zones.push(new Timezone('SAMT-3SAMST,M3.5.0,M10.5.0/3@Europe/Samara', 'Europe/Samara', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/San_Marino', 'Europe/San Marino', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Sarajevo', 'Europe/Sarajevo', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Simferopol', 'Europe/Simferopol', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Skopje', 'Europe/Skopje', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Sofia', 'Europe/Sofia', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Stockholm', 'Europe/Stockholm', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Tallinn', 'Europe/Tallinn', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Tirane', 'Europe/Tirane', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Uzhgorod', 'Europe/Uzhgorod', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Vaduz', 'Europe/Vaduz', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Vatican', 'Europe/Vatican', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Vienna', 'Europe/Vienna', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Vilnius', 'Europe/Vilnius', ''));
	zones.push(new Timezone('VOLT-3VOLST,M3.5.0,M10.5.0/3@Europe/Volgograd', 'Europe/Volgograd', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Warsaw', 'Europe/Warsaw', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Zagreb', 'Europe/Zagreb', ''));
	zones.push(new Timezone('EET-2EEST,M3.5.0/3,M10.5.0/4@Europe/Zaporozhye', 'Europe/Zaporozhye', ''));
	zones.push(new Timezone('CET-1CEST,M3.5.0,M10.5.0/3@Europe/Zurich', 'Europe/Zurich', ''));
	zones.push(new Timezone('EAT-3@Indian/Antananarivo', 'Indian/Antananarivo', ''));
	zones.push(new Timezone('IOT-6@Indian/Chagos', 'Indian/Chagos', ''));
	zones.push(new Timezone('CXT-7@Indian/Christmas', 'Indian/Christmas', ''));
	zones.push(new Timezone('CCT-6:30@Indian/Cocos', 'Indian/Cocos', ''));
	zones.push(new Timezone('EAT-3@Indian/Comoro', 'Indian/Comoro', ''));
	zones.push(new Timezone('TFT-5@Indian/Kerguelen', 'Indian/Kerguelen', ''));
	zones.push(new Timezone('SCT-4@Indian/Mahe', 'Indian/Mahe', ''));
	zones.push(new Timezone('MVT-5@Indian/Maldives', 'Indian/Maldives', ''));
	zones.push(new Timezone('MUT-4@Indian/Mauritius', 'Indian/Mauritius', ''));
	zones.push(new Timezone('EAT-3@Indian/Mayotte', 'Indian/Mayotte', ''));
	zones.push(new Timezone('RET-4@Indian/Reunion', 'Indian/Reunion', ''));
	zones.push(new Timezone('WST11@Pacific/Apia', 'Pacific/Apia', ''));
	zones.push(new Timezone('NZST-12NZDT,M9.5.0,M4.1.0/3@Pacific/Auckland', 'Pacific/Auckland', ''));
	zones.push(new Timezone('CHAST-12:45CHADT,M9.5.0/2:45,M4.1.0/3:45@Pacific/Chatham', 'Pacific/Chatham', ''));
	zones.push(new Timezone('VUT-11@Pacific/Efate', 'Pacific/Efate', ''));
	zones.push(new Timezone('PHOT-13@Pacific/Enderbury', 'Pacific/Enderbury', ''));
	zones.push(new Timezone('TKT10@Pacific/Fakaofo', 'Pacific/Fakaofo', ''));
	zones.push(new Timezone('FJT-12@Pacific/Fiji', 'Pacific/Fiji', ''));
	zones.push(new Timezone('TVT-12@Pacific/Funafuti', 'Pacific/Funafuti', ''));
	zones.push(new Timezone('GALT6@Pacific/Galapagos', 'Pacific/Galapagos', ''));
	zones.push(new Timezone('GAMT9@Pacific/Gambier', 'Pacific/Gambier', ''));
	zones.push(new Timezone('SBT-11@Pacific/Guadalcanal', 'Pacific/Guadalcanal', ''));
	zones.push(new Timezone('ChST-10@Pacific/Guam', 'Pacific/Guam', ''));
	zones.push(new Timezone('HST10@Pacific/Honolulu', 'Pacific/Honolulu', ''));
	zones.push(new Timezone('HST10@Pacific/Johnston', 'Pacific/Johnston', ''));
	zones.push(new Timezone('LINT-14@Pacific/Kiritimati', 'Pacific/Kiritimati', ''));
	zones.push(new Timezone('KOST-11@Pacific/Kosrae', 'Pacific/Kosrae', ''));
	zones.push(new Timezone('MHT-12@Pacific/Kwajalein', 'Pacific/Kwajalein', ''));
	zones.push(new Timezone('MHT-12@Pacific/Majuro', 'Pacific/Majuro', ''));
	zones.push(new Timezone('MART9:30@Pacific/Marquesas', 'Pacific/Marquesas', ''));
	zones.push(new Timezone('SST11@Pacific/Midway', 'Pacific/Midway', ''));
	zones.push(new Timezone('NRT-12@Pacific/Nauru', 'Pacific/Nauru', ''));
	zones.push(new Timezone('NUT11@Pacific/Niue', 'Pacific/Niue', ''));
	zones.push(new Timezone('NFT-11:30@Pacific/Norfolk', 'Pacific/Norfolk', ''));
	zones.push(new Timezone('NCT-11@Pacific/Noumea', 'Pacific/Noumea', ''));
	zones.push(new Timezone('SST11@Pacific/Pago_Pago', 'Pacific/Pago Pago', ''));
	zones.push(new Timezone('PWT-9@Pacific/Palau', 'Pacific/Palau', ''));
	zones.push(new Timezone('PST8@Pacific/Pitcairn', 'Pacific/Pitcairn', ''));
	zones.push(new Timezone('PONT-11@Pacific/Ponape', 'Pacific/Ponape', ''));
	zones.push(new Timezone('PGT-10@Pacific/Port_Moresby', 'Pacific/Port Moresby', ''));
	zones.push(new Timezone('CKT10@Pacific/Rarotonga', 'Pacific/Rarotonga', ''));
	zones.push(new Timezone('ChST-10@Pacific/Saipan', 'Pacific/Saipan', ''));
	zones.push(new Timezone('TAHT10@Pacific/Tahiti', 'Pacific/Tahiti', ''));
	zones.push(new Timezone('GILT-12@Pacific/Tarawa', 'Pacific/Tarawa', ''));
	zones.push(new Timezone('TOT-13@Pacific/Tongatapu', 'Pacific/Tongatapu', ''));
	zones.push(new Timezone('TRUT-10@Pacific/Truk', 'Pacific/Truk', ''));
	zones.push(new Timezone('WAKT-12@Pacific/Wake', 'Pacific/Wake', ''));
	zones.push(new Timezone('WFT-12@Pacific/Wallis', 'Pacific/Wallis', ''));
	
	var sb = [];
	for (var i = 0; i < zones.length; i++) {
		var zone = zones[i];
		sb.push('<option value="');
		sb.push(zone.value);
		sb.push('">');
		sb.push(zone.en);
		sb.push('</option>');
	}
	return sb.join('');
}

