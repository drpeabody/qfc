
let input = `

	  import  stdio ,  string ;

struct vec2 (int x, int y);

void sum(vec2* a,vec2* b){
	a->x += b->x;
	a->y += b->y;
}

int main(){
	vec2 a = vec2   (
		(1 + 3), 
		2), 
	b;
	b.x = 4;
	sum(&a, &b);
	printf("%d\n", a.x);
}


`;

const fs = require('fs');

class Code {
	constructor(code, step){
		this.code = code;
		this.step = step;
	}
}

const STRUCTS = [];

const TOKENS = {
	'import': (input, curline, curIdx) => {
		curline.shift();
		let line = curline
			.filter(s => s !== ',')
			.map(s => s.replace(';', ''))
			.map(s => '#include <' + s + '.h>');
		return new Code(line.join('\n'), 1);
	},
	'struct': (input, curline, curIdx) => {
		curline.shift();
		let name = curline.shift();

		let i = 1;
		while(!curline[curline.length-1].endsWith(');')){
			let next = input[curIdx + (i++)].split(' ');
			curline = curline.concat(next);
		}

		curline[0] = curline[0].replace('(', '');
		curline[curline.length-1] = curline[curline.length-1].replace(')', '');
		curline = curline.map(s => s.replace(',', ';'));

		STRUCTS.push(name);
		let res = 'struct ' + name + ' {' + placeStructs(curline).join(' ') + '};';
		return new Code(res, i);
	},
	'':  (input, curline, curIdx) => {
		STRUCTS.forEach(x => {
			for(let i = 0; i < curline.length; i++){
				if(curline[i].beginsWith(x + '(')){
					
				}
			}
		});
			return new Code(placeStructs(curline).join(' '), 1);
	},

}

function placeStructs(input){
	return input.map(s => {
		STRUCTS.forEach(x => {
			s = s.replace(new RegExp(x.replace('(', '\\('), 'g'), 'struct ' + x);
		})
		return s;
	});
}

function clean(input){
	let arr = input
		.replace(/\s+/g, ' ')
		.replace(/ \(/g, '(')
		.split(/;/);
	return arr
		.map(s => s.trim())
		.filter(s => s !== '')
		.map(s => s + ';');
}

function parse(input){
	let output = [];
	for(let i = 0; i < input.length;){
		let s = input[i].split(' ');
		let ans = undefined;
		if(TOKENS[s[0]]) 
			ans = TOKENS[s[0]](input, s, i);
		else 
			ans = TOKENS[''](input, s, i);
		output.push(ans.code);
		i += ans.step;
		
	};
	return output;
}

compile = () => {
	console.log(
		parse(
		clean(
			input
	)
	)
	.join('\n'));
}

compile();

// input = '';
// if(process.argv.length < 3) throw Error("Illegal Arguments.");
// fs.readFile(process.argv[2], 'utf8', function(err, data) {
// 	if (err) throw err;
// 	input = data;
// 	compile();
// });
