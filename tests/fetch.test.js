import { describe, expect, test } from 'senkora:test';

const data = fetch('https://jsonplaceholder.typicode.com/todos/1');
const data2 = fetch('Senkora'); // Fail

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

describe('fetch - Fail', () => {
    test('status', () => {
        expect(data2.status).toEqual(0);
    });

    test('error', () => {
        expect(data2.error).toEqual('Failed to perform the HTTP request');
    });

    test('ok', () => {
        expect(data2.ok).toBeFalse();
    });
});
