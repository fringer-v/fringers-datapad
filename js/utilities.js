function skillDicePool(characteristic, rank)
{
	var dice = "";
	var i;

	if (rank > characteristic) {
		var t = rank;

		rank = characteristic;
		characteristic = t;
	}
	for (i=0; i<rank; i++)
		dice += "P";
	for (i=0; i<characteristic-rank; i++)
		dice += "A";
	return dice;
}

function sizeWithDefault(size, default_x)
{
	return size == 0 ? default_x : size;
}

function makeTransparent(trans, color)
{
	var res = "#"+trans+color.substr(1);
	return res;
}

function left(str, pat)
{
	var n = str.indexOf(pat);
	if (n < 0)
	   return str;
	return str.substr(0, n);
}

function right(str, pat)
{
	var n = str.indexOf(pat);
	if (n < 0)
	   return "";
	return str.substr(n+1);
}

function endsWith(str, pat)
{
	return str.indexOf(pat, str.length - pat.length) !== -1;
}

function appendList(list, toadd, delim)
{
	if (list.length > 0)
		return list + delim + toadd;
	return toadd;
}

function repeat(pattern, count) {
	var result = '';

	while (count > 0) {
		result += pattern;
		count--;
	}
	return result;
}

String.prototype.replaceAll = function (fin, rep) {
	var str = this;

	while (str.indexOf(fin) >= 0)
		str = str.replace(fin, rep);
	return str;
}
