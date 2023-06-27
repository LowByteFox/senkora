import { describe, expect, test } from 'senkora:test';

const data = fetch('https://jsonplaceholder.typicode.com/todos/1');
const data2 = [
    'Senkora', // not a valid URL
    'google.com', // must be https://google.com
].map(f => fetch(f))

describe('fetch - Success', () => {
    test('status', () => {
        expect(data.status).toEqual(200);
    });

    test('text', () => {
        expect(JSON.parse(data.text)).toEqual({ "userId": 1, "id": 1, "title": "delectus aut autem", "completed": false });
    });

    test('ok', () => {
        expect(data.ok).toBeTrue();
    });
});

describe('fetch - Failure', () => {
    test('status', () => {
        for (const d of data2) {
            expect(d.status).toEqual(0);
        };
    });

    test('error', () => {
        for (const d of data2) {
            expect(d.error).toEqual('Failed to perform the HTTP request');
        };
    });

    test('ok', () => {
        for (const d of data2) {
            expect(d.ok).toBeFalse();
        };
    });
});