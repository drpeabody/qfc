
// Use ful type checking
(() => {
	clog = console.log;

    // Usefule Type Checking Function

	// assert(door, NotEmpty); or assert(NotEmpty, door);
	//		Fails for null, '', undefined
	//		Success for any non empty object
	// assert(door instanceof Door, true);
	//		Checks for objects that were made using new keyword
	// assert(typeof name, 'string')
	//		Checks of type of built in objects
	// assert(typeof foo, 'function')
	// 		Also works

    assert = function(obj1, obj2, msg) {
    	if(obj1 === NotEmpty && !obj2)
    		throw Error(msg);
    	if(obj2 === NotEmpty && !obj1)
    		throw Error(msg);
    	if(obj1 !== obj2 && obj1 !== NotEmpty && obj2 != NotEmpty) 
    		throw Error(msg);
    }

    // A special long unique string to ensure non empty objects using assrt()
    NotEmpty = 'ODH@<)UD@D(@#>DJIJEIDKLADJ<@UD(UD>KLDM:LEDJ';

    // Define constant object members

    final = (obj, key, value) => {
    	assert(obj, NotEmpty, 'Object cannot be empty');
    	assert(key, NotEmpty, 'Key cannot be empty');
		Object.defineProperty( obj, key, {
			value: value,
			writable: false,
			enumerable: true,
			configurable: true
		}); // Makes obj.<key> unwritable and final
    }

    className = (obj) => { 
    	if(typeof obj !== 'object' && typeof obj !== 'function')
    		throw Error('Only objects have Class Name');
    	return obj.constructor.name;
    }
})();


class Token {
	constructor(name, regex, getVal) {
		this.name = name;
		this.regex = regex; 
		this.getVal = getVal;
	}
	matches(char) { 
		return this.regex.test(char); 
	}
	do(input, idx) {
		let res = this.getVal(input, idx, this.regex);
		if(res.value) return {
				'value': { 'type': this.name, 'value': res.value },
				'newIdx': res.newIdx
			};
		else return { 'newIdx': res.newIdx }
	}
}

const TOKENS = [
	new Token('Paren', /[()]/, (input, idx) => { 
		return {'value': input[idx], 'newIdx': idx + 1 };
	}),
	new Token('WhiteSpace', /\s/, (input, idx) => { 
		return {'newIdx': idx + 1 };
	}),
	new Token('Number', /[0-9]/, (input, idx, regex) => {
		let val = '';
		do {
			val += input[idx++];
		} while (regex.test(input[idx]));
		return { 'value': val, 'newIdx': idx};
	}),
	new Token('String', /"/, (input, idx, regex) => {
		let nextIdx = input.indexOf('"', idx + 1);
		return { 'value': input.substring(idx + 1, nextIdx), 'newIdx': nextIdx + 1 };
	}),
	new Token('Name', /[a-z]/, (input, idx, regex) => {
		let val = '';
		do {
			val += input[idx++];
		} while (regex.test(input[idx]));
		return { 'value': val, 'newIdx': idx };
	})
]

function tokeniizer(input) {
	let idx = 0, tokens = [];

	while(idx < input.length) {
		let t = TOKENS.find(token => token.matches(input[idx]));
		if(!t) 
			throw Error('Unexpected Token at index ' + idx);
		let res = t.do(input, idx);
		if(res.value) tokens.push(res.value);
		idx = res.newIdx;
	}
	return tokens;
}

const input  = '(add 2 (subtract 4 2))';


let h = 'something';

console.log({h: 9})

// clog(tokenizer(input));
clog(tokeniizer(input));