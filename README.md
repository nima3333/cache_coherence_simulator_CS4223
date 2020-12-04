# Cache coherence simulator

This repo contains the assignment 2 of CS4223 Multi-core Architectures. The goal was to build from scratch a trace-driven cache coherence simulator to compare multiple protocols.

## Report

The report is avaliable. However, due to one flaw described in the next section, the comparaison between protocols is not accurate.

## Issues

After a meeting with Prof Trevor, we identified 2 flaws in our code :
- The bus is not locked during a transaction. The core is, but the bus is still avaliable to take new messages.
- The Dragon protocol does not follow the specification regarding the share line.

