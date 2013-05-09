var mtrand = require('./build/Release/mtrand');

function ToInteger(x) {
	x = Number(x);
	return x < 0 ? Math.ceil(x) : Math.floor(x);
}

function modulo(a, b) {
	return a - Math.floor(a/b)*b;
}

function ToUint32(x) {
	return modulo(ToInteger(x), Math.pow(2, 32));
}

console.warn(mtrand);
console.warn(ToUint32(mtrand.irand()));
console.warn(mtrand.irand(10));
console.warn(mtrand.irand(0, 40));
console.warn(mtrand.irand(200, 1000));

var randomValues = mtrand.irand_array(16, 0, 100);
console.warn(randomValues);
