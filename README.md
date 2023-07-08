# pr-quadtree

# Overview

A quadtree is a data structure that stores $n$-dimensional points and enables efficient search for the stored points. In the case that $n=2$, a quadtree can be used to store $2$-dimensional points. This is called a point-region quadtree, or PR quadtree.

A binary tree is defined as a finite set of nodes that are either empty, or have a root and two binary trees $T_l$ and $T_r$ (left and right subtree). A quadtree has a similar definition in that instead of two subtrees, there are four subtrees. Each node either has four children (NW, NE, SW, SE) or is a leaf node.

## Building a PR Quadtree

Each node represents a rectangle that covers a part of an area to index. The root node covers the entire area. A PR quadtree is built recursively - starting with an initial rectangle that covers all points, and recursively subdivides a rectangle into four equally sized sub-rectangles whenever a rectangle would contain more than a single point.

Binary trees depends on the order of value insertion, whereas a PR quadtree is independent of the order of datapoint insertion since the nodes are independent from the inserted datapoints (rectangles are determined by the initial rectangle of the root node).

![](https://static.au.edusercontent.com/files/gjfvRRsIh0De3caS0VlFY2AP)


## Search

To enable efficient search, every leaf node in a PR quadtree either contains a single point or no point at all. Every node is either a leaf node or an internal node (contains children). The region of an internal node always contains more than one point.

To search for a single datapoint using its 2D coordinates, check if the point lies in the quadtree. If it does, recursively compute on each level the sub-rectangle that contains the coordinates of the datapoint to be searched until we reach a leaf node. The leaf node either contains the datapoint or it does not.

## Range (Window) Query

This query searches all datapoints in a given query rectangle. It starts at the root and checks which children nodes overlap with the query rectangle, and repeats for all internal children overlapping with the query rectangle until it reaches a leaf level.

## Usage

A makefile is provided to produce the executables `dict3` and `dict4`.

### dict3 - Point-Region Queries

Example input: `./dict3 3 dataset_2.csv output.txt 144.969 -37.7975 144.971 -37.7955`

1. First argument: stage, will always be 3
2. Second argument: filename of data file
3. Third argument: filename of output file
4. Fourth and fifth argument: `x`,`y` / longitude, latitude coordinate pair for bottom-left corner of root node area.
5. Sixth and seventh argument: top right corner of root node area following the same convention.

Example output to file:

```
144.97056424489568 -37.796155887263744 
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
144.96941668057087 -37.79606116572821 
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
144.95538810397605 -37.80355555400948 
--> footpath_id: 19458 || address: Queensberry Street between Capel Street and Howard Street || clue_sa: North Melbourne || asset_type: Road Footway || deltaz: 1.44 || distance: 94.82 || grade1in: 65.8 || mcc_id: 1385878 || mccid_int: 20950 || rlmax: 35.75 || rlmin: 34.31 || segside: North || statusid: 2 || streetid: 1008 || street_group: 20939 || start_lat: -37.803461 || start_lon: 144.954243 || end_lat: -37.803556 || end_lon: 144.955388 ||
```

Example output to stdout:

```
144.97056424489568 -37.796155887263744 --> NE SW NE NE 
144.96941668057087 -37.79606116572821 --> NE SW NE NW 
144.95538810397605 -37.80355555400948 --> SW NW SE
```

### dict4 - Range Queries

Example input: `./dict4 4 dataset_2.csv output.txt 144.968 -37.797 144.977 -37.79`

Example output to file:

```
144.968 -37.797 144.977 -37.79 
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
--> footpath_id: 29996 || address: || clue_sa: Carlton || asset_type: Road Footway || deltaz: 0.46 || distance: 54.51 || grade1in: 118.5 || mcc_id: 1388910 || mccid_int: 0 || rlmax: 24.91 || rlmin: 24.45 || segside: || statusid: 0 || streetid: 0 || street_group: 29996 || start_lat: -37.793272 || start_lon: 144.975507 || end_lat: -37.794367 || end_lon: 144.975315 || 
144.9678 -37.79741 144.97202 -37.79382 
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
144.973 -37.795 144.976 -37.792 
--> footpath_id: 29996 || address: || clue_sa: Carlton || asset_type: Road Footway || deltaz: 0.46 || distance: 54.51 || grade1in: 118.5 || mcc_id: 1388910 || mccid_int: 0 || rlmax: 24.91 || rlmin: 24.45 || segside: || statusid: 0 || streetid: 0 || street_group: 29996 || start_lat: -37.793272 || start_lon: 144.975507 || end_lat: -37.794367 || end_lon: 144.975315 ||
```

Example output to stdout:

```
144.968 -37.797 144.977 -37.79 --> SW SW SE NE SE 
144.9678 -37.79741 144.97202 -37.79382 --> SW SW SE 
144.973 -37.795 144.976 -37.792 --> NE SE
```

## Dataset

From the City of Melbourne Open Data website. The dataset used in this project is a subset of the [Footpath Steepness](https://data.melbourne.vic.gov.au/City-Council/Footpath-steepness/rpt3-2axt) dataset combined with data from the [Small Areas for Census of Land Use and Employment (CLUE)](https://data.melbourne.vic.gov.au/Business/Small-Areas-for-Census-of-Land-Use-and-Employment-/gei8-3w86) dataset. This dataset has been processed to simplify the geometric polygon data into additional attributes `clue_sa`, `start lon`, `start lat`, `end lon`, `end lat`, which can be used to approximate the footpath as a line on the map.

The original dataset has the following geometry:

![](https://static.au.edusercontent.com/files/hTiYK0v7px50uRv8EuM9PAub)
This is then simplified into lines comprising the largest distance between points on the boundary. Note that some curved geometry, such as College Crescent, loses information in this transformation.

![](https://static.au.edusercontent.com/files/uC2zuQkd822SJhktyk088Tbg)
The dataset is sampled for five datasets of increasing scale.

- dataset_1000.csv - 1000 footpath segments

![](https://static.au.edusercontent.com/files/m2MgmeckBpbTtRsS8JrjDGyy)


## Applications

- Maps
