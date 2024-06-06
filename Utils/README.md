# Thesis-System-Utils

## Install required packages

```shell
pip install -r requirements.txt
```

## Setup dataset

Make sure to download the datasets from [here](https://mmaction2.readthedocs.io/en/latest/dataset_zoo/skeleton.html) and place them to its corresponding directory, e.g. `datasets_in/your_dataset.pkl`.

## Usage

### Convert dataset

```shell
python convert_dataset.py --in-dataset-path datasets_in/ntu60_2d.pkl --categories-path label_maps/ntu60.txt --split xsub_val --dir-out datasets_out/ntu60_2d
```

```shell
python convert_dataset.py --in-dataset-path datasets_in/ntu60_3d.pkl --categories-path label_maps/ntu60.txt --split xsub_val --dir-out datasets_out/ntu60_3d
```

```shell
python convert_dataset.py --in-dataset-path datasets_in/ntu120_2d.pkl --categories-path label_maps/ntu120.txt --split xsub_val --dir-out datasets_out/ntu120_2d
```

```shell
python convert_dataset.py --in-dataset-path datasets_in/ntu120_3d.pkl --categories-path label_maps/ntu120.txt --split xsub_val --dir-out datasets_out/ntu120_3d
```

### Convert samples

```shell
set SPLIT="test"
set HC_ANNOTATIONS="stgcn-ntu60-xsub-%SPLIT%-hrnet-j-confidence50-hc"
set HC_ANNOTATIONS_PATH="annotations_in/%HC_ANNOTATIONS%.pkl"
python convert_annotations.py --in-annotations-path %HC_ANNOTATIONS_PATH% --categories-path label_maps/ntu60.txt  --dir-out annotations_out/%HC_ANNOTATIONS%
set LC_ANNOTATIONS="stgcn-ntu60-xsub-%SPLIT%-hrnet-j-confidence50-lc"
set LC_ANNOTATIONS_PATH="annotations_in/%LC_ANNOTATIONS%.pkl"
python convert_annotations.py --in-annotations-path %LC_ANNOTATIONS_PATH% --categories-path label_maps/ntu60.txt  --dir-out annotations_out/%LC_ANNOTATIONS%
```