'use strict';
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const html = fs.readFileSync(path.join(__dirname, '..', 'step-07b.1.html'), 'utf8');
const script = [...html.matchAll(/<script\b[^>]*>([\s\S]*?)<\/script>/gi)].map((match) => match[1]).join('\n');
const comparatorSource = script.match(/function compareLibraryBooks\(a,b,mode\)\{[^\n]*\}/)?.[0];
assert.ok(comparatorSource, 'Prototype shared book comparator exists');
const descendingTabs = [true, false, false];
const compare = vm.runInNewContext(`${comparatorSource}; compareLibraryBooks`, { descendingTabs });
const book = (id, recentOrder, title, author) => ({
  id,
  dataset: { recentOrder: String(recentOrder) },
  querySelector: (selector) => ({ textContent: selector === 'strong' ? title : author }),
});
const fixture = [book('third', 2, 'Zulu', 'C. Author'), book('first', 0, 'Alpha', 'B. Author'), book('second', 1, 'Mike', 'A. Author')];
const sortedIds = (mode) => fixture.slice().sort((a, b) => compare(a, b, mode)).map(({ id }) => id);

assert.deepEqual(sortedIds(0), ['first', 'second', 'third'], 'Recent descending retains original recent order');
descendingTabs[0] = false;
assert.deepEqual(sortedIds(0), ['third', 'second', 'first'], 'Recent ascending reverses original recent order');
assert.deepEqual(sortedIds(1), ['first', 'second', 'third'], 'Title ascending sorts by title');
descendingTabs[1] = true;
assert.deepEqual(sortedIds(1), ['third', 'second', 'first'], 'Title descending reverses title order');
assert.deepEqual(sortedIds(2), ['second', 'first', 'third'], 'Author ascending sorts by author');
descendingTabs[2] = true;
assert.deepEqual(sortedIds(2), ['third', 'first', 'second'], 'Author descending reverses author order');

console.log('Library sort regression checks passed.');
