# Packup: Load Distribution Optimizer for Mountain Expeditions

An R package written in C designed to fairly distribute shared equipment and food supplies among expedition participants based on their physical strength and trip duration.

## The Problem: Load Distribution

During multi-day mountain trekking, distributing shared gear and food supplies fairly among participants with varying physical capacities is a notorious challenge. Done poorly, it leads to exhaustion, pacing issues, and well-deserved complaints on the trail.

In this problem we have two types of cargo

| Cargo Type | Description | Example |
| :--- | :--- | :--- |
| **Shared equipment** | Essential gear that remains in the backpack for the entire duration of the trip. | Saw, cooking pot, group first-aid kit. |
| **Food packages** | Consumable supplies that only weigh down a participant until the day they are consumed. | Day 3 breakfast, Day 2 dinner. |

**Constraint:** Packing occurs exactly once before the trip starts. There is no option for cargo redistribution during the expedition.

## Mathematical Model

We formulate this as a **minimax optimization problem**, aiming to minimize the maximum cumulative effort experienced by any individual participant.

* Let $M = \{m_1, m_2, \dots, m_n\}$ be the set of item **masses/weights**.
* Let $T = \{t_1, t_2, \dots, t_n\}$ be the **durations** (in days) for each item (e.g., food for Day 2 has a duration of 2).
* Let $S = \{s_1, s_2, \dots, s_m\}$ be the **strength coefficients** of the participants.

### Objective Function
The goal is to find a mapping $m_i \rightarrow s_j$ that minimizes the maximum individual load metric:

$$\min \left( \max_{j} (\text{load}_j) \right)$$

Where the accumulated effort for a participant $j$ is defined as:

$$\text{load}_j = \sum_{i \in \text{Assigned}_j} \frac{m_i \times t_i}{s_j}$$

---

## The Solution: Heap-Based Greedy Algorithm (Min-Heap)

1. **Pre-sorting:** Items are sorted in descending order primarily by their **duration** ($T$), and secondarily by their **weight** ($M$).
2. **Min-Heap Tracking:** A binary Min-Heap keeps track of the participants. In each iteration, the algorithm pops the person with the current *lowest cumulative effort*.
3. **Greedy Assignment:** The heaviest/longest-lasting remaining item is assigned to this person. Their effort metric is updated, and they are pushed back into the heap.

---

Here is a practical simulation of how the package optimizes load balancing using an example with 3 hikers of varying strengths: **Kasia** ($0.75$), **Basia** ($1.0$), and **Krysia** ($1.2$).

### R Implementation

```r
library(packup)

# Define expedition setup
item_names <- c("dinner1", "lunch1","breakfast2", "dinner2", "lunch2", "axe", "pot")
weights <- c(0.6,0.5,1,0.8,0.7,2,3)
durations <- as.integer(c(1,1,2,2,2,2,2))

person_names <- c("Kasia", "Basia", "Krysia")
strengths <- c(0.75,1,1.2)

# Run the optimization
results <- assign_items(weights, durations, strengths)

# View the balanced result frame
result_df <- data.frame(
  item        = item_names,
  weight      = weights,
  duration    = durations,
  assigned_to = person_names[results],
  strength    = strengths[results]
)

print(result_df)

```

#### Result

| | Item Name | Weight ($m_i$) | Duration ($t_i$) | Assigned Hiker | Hiker Strength ($s_j$) |
| :-: | :--- | :---: | :---: | :--- | :---: |
| **1** | dinner1 | 0.6 kg | 1 day | **Kasia** | 0.75 |
| **2** | lunch1 | 0.5 kg | 1 day | **Krysia** | 1.20 |
| **3** | breakfast2 | 1.0 kg | 2 days | **Kasia** | 0.75 |
| **4** | dinner2 | 0.8 kg | 2 days | **Kasia** | 0.75 |
| **5** | lunch2 | 0.7 kg | 2 days | **Basia** | 1.00 |
| **6** | axe | 2.0 kg | 2 days | **Basia** | 1.00 |
| **7** | pot | 3.0 kg | 2 days | **Krysia** | 1.20 |

* **Basia's Effort:** $5.40$
* **Krysia's Effort:** $5.42$
* **Kasia's Effort:** $5.60$

--- 
### Installation

You can install the development version of `packup` directly from GitHub using the `devtools` package. 

Open your R console and run the following commands:

```R
# If you don't have devtools installed yet:
install.packages("devtools")

# Install packup from GitHub
devtools::install_github("foggy-froggie/C-project")
```