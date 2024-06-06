import argparse
import io
import os

import numpy as np
import torch
from tqdm.rich import tqdm_rich


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument("--in-annotations-path", required=True)
    parser.add_argument("--categories-path", required=True)
    parser.add_argument("--dir-out", required=True)

    return parser.parse_args()


def process_dataset(in_annotations_path, categories_path, dir_out):
    with open(categories_path, "r") as file:
        categories = file.readlines()
        categories = tuple([category.strip() for category in categories])
        dataset_name = os.path.basename(categories_path).split(".")[0]

    annotations = [
        annotation
        for annotations_cat in np.load(in_annotations_path, allow_pickle=True)
        for annotation in annotations_cat
    ]

    for sample in tqdm_rich(annotations):
        label = sample["label"]
        label_dir = os.path.join(dir_out, str(label))

        if not os.path.exists(label_dir):
            os.makedirs(label_dir)

        p = os.path.join(label_dir, f"{sample['frame_dir']}.pkl")
        f = io.BytesIO()

        new_sample = dict(
            frame_dir=sample["frame_dir"],
            label=torch.tensor(label),
            keypoint=torch.tensor(sample["keypoint"]),
            total_frames=torch.tensor(sample["total_frames"]),
            dataset_name=dataset_name,
            categories=categories,
        )

        if "img_shape" in sample:
            new_sample["img_shape"] = torch.tensor(sample["img_shape"])
        if "original_shape" in sample:
            new_sample["original_shape"] = torch.tensor(sample["original_shape"])
        if "keypoint_score" in sample:
            new_sample["keypoint_score"] = torch.tensor(sample["keypoint_score"])

        torch.save(new_sample, f)

        with open(p, "wb") as outfile:
            outfile.write(f.getbuffer())


def main():
    args = parse_args()

    process_dataset(args.in_annotations_path, args.categories_path, args.dir_out)


if __name__ == "__main__":
    main()
