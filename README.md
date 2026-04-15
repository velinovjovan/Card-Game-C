# TAC - Two Player Card Game
A command-line two-player card game written in C where players take turns drawing and placing cards, trying to be the first to empty their hand. The deck is shuffled using a **Blum Blum Shub** pseudorandom number generator seeded by the player.

---

## How It Works

The deck consists of 52 cards (values 1–13, four copies each) and is split evenly between the two players — 26 cards each, placed face-down in their **closed pile**. Each player also has an **open pile** and there is a shared **center pile**.

On each turn the active player:
1. Draws a card — either from the top of their closed pile or from their open pile
2. Places it — either onto the center, onto the opponent's open pile, or onto their own open pile (ending their turn)

The game ends when a player empties both their closed and open piles.

---

## Placement Rules

Cards can only be placed on a pile if the card being placed is exactly **one higher** than the top card of that pile (wrapping: 13 → 1 via modulo):

If a placement onto the center or the opponent's pile is invalid, the player must place the card on their own open pile instead, passing the turn.

---

## BBS Pseudorandom Shuffle

The deck is shuffled using the **Blum Blum Shub** algorithm — a cryptographically secure PRNG based on modular squaring:

```
x(n+1) = x(n)² mod m       where m = p × q
```

- `p = 24419`, `q = 29243` (both safe primes), `m = 714084817`
- Each iteration extracts the **parity bit** of the result
- 6 iterations are run per call to produce one random byte

The player provides the seed manually at startup.

---

---

## Building & Running

Requires C99 or later and a compiler that supports `__uint128_t` (GCC, Clang).
